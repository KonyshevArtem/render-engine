#ifdef RENDER_BACKEND_DX12

#include "graphics_backend_api.h"
#include "graphics_backend_api_dx12.h"
#include "enums/texture_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/fence_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "types/graphics_backend_profiler_marker.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_program_descriptor.h"
#include "helpers/dx12_helpers.h"
#include "debug.h"

#include <unordered_map>

namespace DX12Local
{
    inline void ThrowIfFailed(HRESULT result);

    ID3D12Device5* s_Device;

    struct ResourceData
    {
        ID3D12Resource* Resource;
        D3D12_RESOURCE_STATES State;
    };

    struct RenderTargetState
    {
        D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;
        TextureInternalFormat Format;
        ResourceData* ResourceData;
        bool IsLinear;
        bool IsEnabled;
        bool NeedClear;
    };

    struct ShaderObject
    {
        ID3D12PipelineState* PSO;
        ID3D12RootSignature* RootSignature;
        std::unordered_map<size_t, int> ResourceBindingHashToRootParameterIndex;
    };

    struct GeometryData
    {
        D3D12_VERTEX_BUFFER_VIEW* VertexBufferView;
        D3D12_INDEX_BUFFER_VIEW* IndexBufferView;
    };

    struct SamplerData
    {
        D3D12_SAMPLER_DESC SamplerDesc;
    };

    struct CommandList
    {
        ID3D12GraphicsCommandList6* List;
        ID3D12CommandAllocator* Allocator;
        ID3D12CommandQueue* Queue;

        CommandList(){}

        CommandList(ID3D12CommandQueue* commandQueue, D3D12_COMMAND_LIST_TYPE type, const std::string& name)
        {
            Queue = commandQueue;

            ThrowIfFailed(DX12Local::s_Device->CreateCommandAllocator(type, IID_PPV_ARGS(&Allocator)));
            ThrowIfFailed(DX12Local::s_Device->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&List)));

            std::string allocatorName = name + "_Allocator";
            ThrowIfFailed(Allocator->SetPrivateData(WKPDID_D3DDebugObjectName, allocatorName.size(), allocatorName.c_str()));
            ThrowIfFailed(List->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str()));
        }

        void Execute()
        {
            Close();

            ID3D12CommandList* commandLists[] = {List};
            Queue->ExecuteCommandLists(1, commandLists);

            Reset();
        }

        void Reset() const
        {
            ThrowIfFailed(List->Reset(Allocator, nullptr));
        }

        void Close() const
        {
            ThrowIfFailed(List->Close());
        }

        void InitNewFrame()
        {
            ThrowIfFailed(Allocator->Reset());
            Reset();
        }

        ID3D12GraphicsCommandList6* operator->() const
        {
            return List;
        }
    };

    struct PerFrameData
    {
        CommandList RenderCommandList;
        CommandList CopyCommandList;

        ID3D12DescriptorHeap* ColorTargetDescriptorHeap;
        uint32_t ColorTargetHeapIndex;

        ID3D12DescriptorHeap* DepthTargetDescriptorHeap;
        uint32_t DepthTargetHeapIndex;

        ID3D12DescriptorHeap* ResourceDescriptorHeap;
        uint32_t ResourceDescriptorHeapIndex;

        ID3D12DescriptorHeap* SamplerDescriptorHeap;
        uint32_t SamplerDescriptorHeapIndex;

        ID3D12Fence* RenderQueueFence;
        ID3D12Fence* CopyQueueFence;
        HANDLE RenderQueueFenceEvent;
        HANDLE CopyQueueFenceEvent;
        uint64_t FenceValue;
    };

    constexpr TextureInternalFormat k_SwapChainColorFormat = TextureInternalFormat::RGBA8;
    constexpr TextureInternalFormat k_SwapChainDepthFormat = TextureInternalFormat::DEPTH_24_STENCIL_8;

    HWND s_Window;
    IDXGISwapChain4* s_SwapChain;
    int s_SwapChainWidth;
    int s_SwapChainHeight;

    ID3D12CommandQueue* s_RenderQueue;
    ID3D12CommandQueue* s_CopyQueue;
    ID3D12InfoQueue* s_InfoQueue;

    ID3D12DescriptorHeap* s_ColorBackbufferDescriptorHeap;
    ID3D12DescriptorHeap* s_DepthBackbufferDescriptorHeap;
    ID3D12DescriptorHeap* s_ImGuiDescriptorHeap;

    UINT s_ColorTargetDescriptorSize;
    UINT s_DepthTargetDescriptorSize;
    UINT s_ResourceDescriptorSize;
    UINT s_SamplerDescriptorSize;

    float s_ClearColor[4] = {0, 0, 0, 0};
    float s_ClearDepth = 1;
    RenderTargetState s_RenderTargetStates[static_cast<int>(FramebufferAttachment::MAX)];

    D3D12_VIEWPORT s_CurrentViewport;
    D3D12_RECT s_CurrentScissorsRect;

    ResourceData s_ColorBackbuffers[GraphicsBackend::GetMaxFramesInFlight()];
    ResourceData s_DepthBackbuffers[GraphicsBackend::GetMaxFramesInFlight()];
    PerFrameData s_PerFrameData[GraphicsBackend::GetMaxFramesInFlight()];

    ShaderObject* s_CurrentShaderObject;

    void GetHardwareAdapter(IDXGIFactory7* pFactory, IDXGIAdapter4** ppAdapter)
    {
        for (UINT i = 0; pFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(ppAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC3 desc;
            (*ppAdapter)->GetDesc3(&desc);
            if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
                continue;

            wprintf(L"Adapter: %s\n", desc.Description);
            break;
        }
    }

    void ResetRenderTargetStates()
    {
        constexpr int max = static_cast<int>(FramebufferAttachment::MAX);
        for (int i = 0; i < max; ++i)
        {
            const FramebufferAttachment attachment = static_cast<FramebufferAttachment>(i);

            RenderTargetState& state = s_RenderTargetStates[i];
            state.IsEnabled = attachment == FramebufferAttachment::COLOR_ATTACHMENT0;
            state.NeedClear = false;
        }
    }

    void PrintInfoQueueMessages()
    {
        if (!s_InfoQueue)
            return;

        const auto GetMessageCategoryName = [](const D3D12_MESSAGE_CATEGORY category) -> const char*
        {
            switch (category)
            {
                case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:
                    return "Application Defined";
                case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:
                    return "Miscellaneous";
                case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
                    return "Initialization";
                case D3D12_MESSAGE_CATEGORY_CLEANUP:
                    return "Cleanup";
                case D3D12_MESSAGE_CATEGORY_COMPILATION:
                    return "Compilation";
                case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
                    return "State Creation";
                case D3D12_MESSAGE_CATEGORY_STATE_SETTING:
                    return "State Setting";
                case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
                    return "State Getting";
                case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
                    return "Resource Manipulation";
                case D3D12_MESSAGE_CATEGORY_EXECUTION:
                    return "Execution";
                case D3D12_MESSAGE_CATEGORY_SHADER:
                    return "Shader";
            }
        };

        UINT64 messageCount = s_InfoQueue->GetNumStoredMessages();
        for (UINT64 i = 0; i < messageCount; ++i)
        {
            SIZE_T messageLength = 0;
            s_InfoQueue->GetMessage(i, nullptr, &messageLength);
            D3D12_MESSAGE* message = (D3D12_MESSAGE*)malloc(messageLength);
            s_InfoQueue->GetMessage(i, message, &messageLength);

            switch (message->Severity)
            {
                case D3D12_MESSAGE_SEVERITY_CORRUPTION:
                case D3D12_MESSAGE_SEVERITY_ERROR:
                    Debug::LogErrorFormat("[GraphicsBackend] [{}] {}", GetMessageCategoryName(message->Category), message->pDescription);
                    break;
                case D3D12_MESSAGE_SEVERITY_WARNING:
                    Debug::LogWarningFormat("[GraphicsBackend] [{}] {}", GetMessageCategoryName(message->Category), message->pDescription);
                    break;
                case D3D12_MESSAGE_SEVERITY_INFO:
                case D3D12_MESSAGE_SEVERITY_MESSAGE:
                    Debug::LogInfoFormat("[GraphicsBackend] [{}] {}", GetMessageCategoryName(message->Category), message->pDescription);
                    break;
            }

            free(message);
        }

        s_InfoQueue->ClearStoredMessages();
    }

    PerFrameData& GetCurrentFrameData()
    {
        return s_PerFrameData[GraphicsBackend::GetInFlightFrameIndex()];
    }

    ResourceData& GetCurrentColorBackbuffer()
    {
        return s_ColorBackbuffers[s_SwapChain->GetCurrentBackBufferIndex()];
    }

    ResourceData& GetCurrentDepthBackbuffer()
    {
        return s_DepthBackbuffers[s_SwapChain->GetCurrentBackBufferIndex()];
    }

    uint32_t GetShaderRegister(const GraphicsBackendResourceBindings& bindings)
    {
        return bindings.VertexIndex != -1 ? bindings.VertexIndex : bindings.FragmentIndex;
    }

    D3D12_SHADER_VISIBILITY GetShaderVisibility(const GraphicsBackendResourceBindings& bindings)
    {
        const bool isVertex = bindings.VertexIndex != -1;
        const bool isFragment = bindings.FragmentIndex != -1;

        if (isVertex && isFragment)
            return D3D12_SHADER_VISIBILITY_ALL;
        return isVertex ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL;
    }

    size_t HashCombine(size_t hashA, size_t hashB)
    {
        // boost::hashCombine
        return hashA ^ (hashB + 0x9e3779b9 + (hashA << 6) + (hashA >> 2));
    }

    size_t GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE parameterType, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, const GraphicsBackendResourceBindings& bindings)
    {
        size_t hash = 0;
        hash = HashCombine(hash, std::hash<D3D12_ROOT_PARAMETER_TYPE>{}(parameterType));
        hash = HashCombine(hash, std::hash<D3D12_DESCRIPTOR_RANGE_TYPE>{}(rangeType));
        hash = HashCombine(hash, std::hash<D3D12_SHADER_VISIBILITY>{}(GetShaderVisibility(bindings)));
        hash = HashCombine(hash, std::hash<uint32_t>{}(GetShaderRegister(bindings)));
        return hash;
    }

    void GetWindowSize(HWND window, int& outWidth, int& outHeight)
    {
        RECT rect;
        GetWindowRect(window, &rect);
        outWidth = rect.right - rect.left;
        outHeight = rect.bottom - rect.top;
    }

    inline void ThrowIfFailed(HRESULT result)
    {
        if (FAILED(result))
        {
            PrintInfoQueueMessages();

            const uint32_t resultCode = static_cast<uint32_t>(result);
            if (resultCode == 0x887A0005)
            {
                const uint32_t removeReasonCode = static_cast<uint32_t>(s_Device->GetDeviceRemovedReason());
                Debug::LogErrorFormat("[GraphicsBackend] Device removed with reason {:#08X}", removeReasonCode);
            }
            else
                Debug::LogErrorFormat("[GraphicsBackend] Failed with HRESULT {:#08X}", resultCode);

            throw std::exception();
        }
    }

    void CreateBackbufferResourcesAndViews()
    {
        for (UINT i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE colorBackbufferHandle(s_ColorBackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, s_ColorTargetDescriptorSize);
            ThrowIfFailed(s_SwapChain->GetBuffer(i, IID_PPV_ARGS(&s_ColorBackbuffers[i].Resource)));
            s_Device->CreateRenderTargetView(s_ColorBackbuffers[i].Resource, nullptr, colorBackbufferHandle);
            s_ColorBackbuffers[i].State = D3D12_RESOURCE_STATE_RENDER_TARGET;

            D3D12_RESOURCE_STATES depthBufferState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            DXGI_FORMAT format = DX12Helpers::ToTextureInternalFormat(k_SwapChainDepthFormat, true);
            CD3DX12_CLEAR_VALUE clearValue(format, 1, 0);
            D3D12_RESOURCE_DESC depthBackbufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, s_SwapChainWidth, s_SwapChainHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            D3D12_HEAP_PROPERTIES depthBackbufferHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(s_Device->CreateCommittedResource(&depthBackbufferHeapProps, D3D12_HEAP_FLAG_NONE, &depthBackbufferDesc, depthBufferState, &clearValue, IID_PPV_ARGS(&s_DepthBackbuffers[i].Resource)));
            CD3DX12_CPU_DESCRIPTOR_HANDLE depthBackbufferHandle(s_DepthBackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, s_DepthTargetDescriptorSize);
            s_Device->CreateDepthStencilView(s_DepthBackbuffers[i].Resource, nullptr, depthBackbufferHandle);
            s_DepthBackbuffers[i].State = depthBufferState;

            std::string depthBackbufferName = "DepthBackbuffer_" + std::to_string(i);
            s_DepthBackbuffers[i].Resource->SetPrivateData(WKPDID_D3DDebugObjectName, depthBackbufferName.size(), depthBackbufferName.c_str());
        }
    }

    void CreateSwapChain(HWND window, IDXGIFactory7* factory)
    {
        GetWindowSize(window, s_SwapChainWidth, s_SwapChainHeight);

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
        swapChainDesc.BufferCount = GraphicsBackend::GetMaxFramesInFlight();
        swapChainDesc.Width = s_SwapChainWidth;
        swapChainDesc.Height = s_SwapChainHeight;
        swapChainDesc.Format = DX12Helpers::ToTextureInternalFormat(k_SwapChainColorFormat, true);
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain1* swapChain;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
                s_RenderQueue,
                window,
                &swapChainDesc,
                nullptr,
                nullptr,
                &swapChain
        ));
        ThrowIfFailed(swapChain->QueryInterface(__uuidof(IDXGISwapChain4), reinterpret_cast<void**>(&s_SwapChain)));
        swapChain->Release();

        CreateBackbufferResourcesAndViews();
    }

    void WaitForFrameEnd(PerFrameData& frameData)
    {
        uint64_t expectedFenceValue = frameData.FenceValue - 1;
        if (frameData.RenderQueueFence->GetCompletedValue() < expectedFenceValue || frameData.CopyQueueFence->GetCompletedValue() < expectedFenceValue)
        {
            ThrowIfFailed(frameData.RenderQueueFence->SetEventOnCompletion(expectedFenceValue, frameData.RenderQueueFenceEvent));
            ThrowIfFailed(frameData.CopyQueueFence->SetEventOnCompletion(expectedFenceValue, frameData.CopyQueueFenceEvent));

            HANDLE events[2] = {frameData.RenderQueueFenceEvent, frameData.CopyQueueFenceEvent};
            WaitForMultipleObjects(1, events, true, INFINITE);
        }
    }

    ID3D12GraphicsCommandList* GetCommandList(PerFrameData& frameData, GPUQueue queue)
    {
        switch (queue)
        {
            case GPUQueue::RENDER:
                return frameData.RenderCommandList.List;
            case GPUQueue::COPY:
                return frameData.CopyCommandList.List;
            default:
                return nullptr;
        }
    }
}

#define ThrowIfFailed DX12Local::ThrowIfFailed

void GraphicsBackendDX12::Init(void* data)
{
    ID3D12Debug3* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        debugController->EnableDebugLayer();

    IDXGIFactory7* factory;
    ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)));

    IDXGIAdapter4* adapter;
    DX12Local::GetHardwareAdapter(factory, &adapter);

    ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&DX12Local::s_Device)));
    ThrowIfFailed(DX12Local::s_Device->QueryInterface(IID_PPV_ARGS(&DX12Local::s_InfoQueue)));

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(DX12Local::s_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&DX12Local::s_RenderQueue)));

    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    ThrowIfFailed(DX12Local::s_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&DX12Local::s_CopyQueue)));

    std::string renderQueueName = "RenderQueue";
    std::string copyQueueName = "CopyQueue";
    ThrowIfFailed(DX12Local::s_RenderQueue->SetPrivateData(WKPDID_D3DDebugObjectName, renderQueueName.size(), renderQueueName.c_str()));
    ThrowIfFailed(DX12Local::s_CopyQueue->SetPrivateData(WKPDID_D3DDebugObjectName, copyQueueName.size(), copyQueueName.c_str()));

    D3D12_DESCRIPTOR_HEAP_DESC colorBackbufferDescriptorHeapDesc{};
    colorBackbufferDescriptorHeapDesc.NumDescriptors = GraphicsBackend::GetMaxFramesInFlight();
    colorBackbufferDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    colorBackbufferDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&colorBackbufferDescriptorHeapDesc, IID_PPV_ARGS(&DX12Local::s_ColorBackbufferDescriptorHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC depthBackbufferDescriptorHeapDesc{};
    depthBackbufferDescriptorHeapDesc.NumDescriptors = GraphicsBackend::GetMaxFramesInFlight();
    depthBackbufferDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    depthBackbufferDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&depthBackbufferDescriptorHeapDesc, IID_PPV_ARGS(&DX12Local::s_DepthBackbufferDescriptorHeap)));

    DX12Local::s_ColorTargetDescriptorSize = DX12Local::s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    DX12Local::s_DepthTargetDescriptorSize = DX12Local::s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    DX12Local::s_ResourceDescriptorSize = DX12Local::s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DX12Local::s_SamplerDescriptorSize = DX12Local::s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    DX12Local::s_Window = *static_cast<HWND*>(data);
    DX12Local::CreateSwapChain(DX12Local::s_Window, factory);

    for (UINT i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        DX12Local::PerFrameData& frameData = DX12Local::s_PerFrameData[i];

        frameData.RenderCommandList = DX12Local::CommandList(DX12Local::s_RenderQueue, D3D12_COMMAND_LIST_TYPE_DIRECT, "RenderCommandList");
        frameData.CopyCommandList = DX12Local::CommandList(DX12Local::s_CopyQueue, D3D12_COMMAND_LIST_TYPE_COPY, "CopyCommandList");

        D3D12_DESCRIPTOR_HEAP_DESC resourceHeapDesc{};
        resourceHeapDesc.NumDescriptors = 1024;
        resourceHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        resourceHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&resourceHeapDesc, IID_PPV_ARGS(&frameData.ResourceDescriptorHeap)));

        D3D12_DESCRIPTOR_HEAP_DESC samplerDescriptorHeapDesc{};
        samplerDescriptorHeapDesc.NumDescriptors = 1024;
        samplerDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        samplerDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&samplerDescriptorHeapDesc, IID_PPV_ARGS(&frameData.SamplerDescriptorHeap)));

        D3D12_DESCRIPTOR_HEAP_DESC colorTargetHeapDesc{};
        colorTargetHeapDesc.NumDescriptors = 1024;
        colorTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        colorTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&colorTargetHeapDesc, IID_PPV_ARGS(&frameData.ColorTargetDescriptorHeap)));

        D3D12_DESCRIPTOR_HEAP_DESC depthTargetHeapDesc{};
        depthTargetHeapDesc.NumDescriptors = 1024;
        depthTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        depthTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&depthTargetHeapDesc, IID_PPV_ARGS(&frameData.DepthTargetDescriptorHeap)));

        DX12Local::s_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameData.RenderQueueFence));
        DX12Local::s_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameData.CopyQueueFence));
        frameData.RenderQueueFenceEvent = CreateEvent(nullptr, false, false, nullptr);
        frameData.CopyQueueFenceEvent = CreateEvent(nullptr, false, false, nullptr);
        frameData.FenceValue = 1;
    }

    DX12Local::ResetRenderTargetStates();

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    frameData.RenderCommandList.Reset();
    frameData.CopyCommandList.Reset();

    CD3DX12_RESOURCE_BARRIER backbufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(DX12Local::GetCurrentColorBackbuffer().Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    frameData.RenderCommandList->ResourceBarrier(1, &backbufferBarrier);
}

GraphicsBackendName GraphicsBackendDX12::GetName()
{
    return GraphicsBackendName::DX12;
}

void GraphicsBackendDX12::InitNewFrame()
{
    GraphicsBackendBase::InitNewFrame();

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    DX12Local::WaitForFrameEnd(frameData);

    int windowWidth;
    int windowHeight;
    DX12Local::GetWindowSize(DX12Local::s_Window, windowWidth, windowHeight);
    if (windowWidth != DX12Local::s_SwapChainWidth || windowHeight != DX12Local::s_SwapChainHeight)
    {
        for (DX12Local::PerFrameData& otherFrameData: DX12Local::s_PerFrameData)
            DX12Local::WaitForFrameEnd(otherFrameData);

        for (DX12Local::ResourceData& backbuffer: DX12Local::s_ColorBackbuffers)
            backbuffer.Resource->Release();

        for (DX12Local::ResourceData& backbuffer: DX12Local::s_DepthBackbuffers)
            backbuffer.Resource->Release();

        DXGI_FORMAT format = DX12Helpers::ToTextureInternalFormat(DX12Local::k_SwapChainColorFormat, true);
        DX12Local::s_SwapChain->ResizeBuffers(GraphicsBackend::GetMaxFramesInFlight(), windowWidth, windowHeight, format, 0);
        DX12Local::s_SwapChainWidth = windowWidth;
        DX12Local::s_SwapChainHeight = windowHeight;

        DX12Local::CreateBackbufferResourcesAndViews();
    }

    frameData.RenderCommandList.InitNewFrame();
    frameData.CopyCommandList.InitNewFrame();

    CD3DX12_RESOURCE_BARRIER backbufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(DX12Local::GetCurrentColorBackbuffer().Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    frameData.RenderCommandList->ResourceBarrier(1, &backbufferBarrier);

    frameData.ResourceDescriptorHeapIndex = 0;
    frameData.SamplerDescriptorHeapIndex = 0;
    frameData.ColorTargetHeapIndex = 0;
    frameData.DepthTargetHeapIndex = 0;
}

void GraphicsBackendDX12::FillImGuiInitData(void* data)
{
    struct InitDataDX12
    {
        void* Window;
        ID3D12Device* Device;
        int MaxFramesInFlight;
        DXGI_FORMAT Format;
        ID3D12DescriptorHeap* DescriptorHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptorHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptorHandle;
    };

    InitDataDX12* initData = reinterpret_cast<InitDataDX12*>(data);
    initData->Window = DX12Local::s_Window;
    initData->Device = DX12Local::s_Device;
    initData->MaxFramesInFlight = GraphicsBackend::GetMaxFramesInFlight();
    initData->Format = DX12Helpers::ToTextureInternalFormat(DX12Local::k_SwapChainColorFormat, true);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 64;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(DX12Local::s_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&DX12Local::s_ImGuiDescriptorHeap)));

    initData->DescriptorHeap = DX12Local::s_ImGuiDescriptorHeap;
    initData->CpuDescriptorHandle = initData->DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    initData->GpuDescriptorHandle = initData->DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void GraphicsBackendDX12::FillImGuiFrameData(void *data)
{
    struct FrameDataDX12
    {
        ID3D12GraphicsCommandList* CommandList;
    };

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    D3D12_CPU_DESCRIPTOR_HANDLE backbufferHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DX12Local::s_ColorBackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), DX12Local::s_SwapChain->GetCurrentBackBufferIndex(), DX12Local::s_ColorTargetDescriptorSize);

    frameData.RenderCommandList->OMSetRenderTargets(1, &backbufferHandle, false, nullptr);
    frameData.RenderCommandList->SetDescriptorHeaps(1, &DX12Local::s_ImGuiDescriptorHeap);

    FrameDataDX12* imguiData = reinterpret_cast<FrameDataDX12*>(data);
    imguiData->CommandList = frameData.RenderCommandList.List;
}

GraphicsBackendTexture GraphicsBackendDX12::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name)
{
    DXGI_FORMAT dxFormat = DX12Helpers::ToTextureInternalFormat(format, isLinear);

    D3D12_CLEAR_VALUE clearValue;
    D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
    if (isRenderTarget)
    {
        if (IsDepthFormat(format))
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            clearValue = CD3DX12_CLEAR_VALUE(dxFormat, DX12Local::s_ClearDepth, 0);
        }
        else
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            state = D3D12_RESOURCE_STATE_RENDER_TARGET;
            clearValue = CD3DX12_CLEAR_VALUE(dxFormat, DX12Local::s_ClearColor);
        }
    }

    switch (dxFormat)
    {
        case DXGI_FORMAT_D16_UNORM:
            dxFormat = DXGI_FORMAT_R16_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            dxFormat = DXGI_FORMAT_R32_TYPELESS;
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            dxFormat = DXGI_FORMAT_R24G8_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            dxFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
            break;
    }

    depth = max(depth, 1);
    D3D12_RESOURCE_DESC desc;
    switch (type)
    {
        case TextureType::TEXTURE_1D:
        case TextureType::TEXTURE_1D_ARRAY:
            desc = CD3DX12_RESOURCE_DESC::Tex1D(dxFormat, width, depth, mipLevels, flags);
            break;
        case TextureType::TEXTURE_2D:
        case TextureType::TEXTURE_2D_MULTISAMPLE:
        case TextureType::TEXTURE_2D_ARRAY:
        case TextureType::TEXTURE_2D_MULTISAMPLE_ARRAY:
            desc = CD3DX12_RESOURCE_DESC::Tex2D(dxFormat, width, height, depth, mipLevels, 1, 0, flags);
            break;
        case TextureType::TEXTURE_CUBEMAP:
        case TextureType::TEXTURE_CUBEMAP_ARRAY:
            desc = CD3DX12_RESOURCE_DESC::Tex2D(dxFormat, width, height, 6 * depth, mipLevels, 1, 0, flags);
            break;
        case TextureType::TEXTURE_3D:
            desc = CD3DX12_RESOURCE_DESC::Tex3D(dxFormat, width, height, depth, mipLevels, flags);
            break;
        case TextureType::TEXTURE_RECTANGLE:
        case TextureType::TEXTURE_BUFFER:
            break;
    }

    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ID3D12Resource* dxTexture;
    ThrowIfFailed(DX12Local::s_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, state, isRenderTarget ? &clearValue : nullptr, IID_PPV_ARGS(&dxTexture)));

    dxTexture->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());

    DX12Local::ResourceData* resourceData = new DX12Local::ResourceData();
    resourceData->Resource = dxTexture;
    resourceData->State = state;

    GraphicsBackendTexture texture;
    texture.Texture = reinterpret_cast<uint64_t>(resourceData);
    texture.Format = format;
    texture.Type = type;
    texture.IsLinear = isLinear;
    return texture;
}

GraphicsBackendSampler GraphicsBackendDX12::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, const std::string& name)
{
    D3D12_TEXTURE_ADDRESS_MODE dxWrapMode = DX12Helpers::ToTextureWrapMode(wrapMode);

    D3D12_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = DX12Helpers::ToTextureFilterMode(filteringMode);
    samplerDesc.AddressU = dxWrapMode;
    samplerDesc.AddressV = dxWrapMode;
    samplerDesc.AddressW = dxWrapMode;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.BorderColor[0] = borderColor[0];
    samplerDesc.BorderColor[1] = borderColor[1];
    samplerDesc.BorderColor[2] = borderColor[2];
    samplerDesc.BorderColor[3] = borderColor[3];
    samplerDesc.MinLOD = minLod;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    DX12Local::SamplerData* samplerData = new DX12Local::SamplerData();
    samplerData->SamplerDesc = samplerDesc;

    GraphicsBackendSampler sampler{};
    sampler.Sampler = reinterpret_cast<uint64_t>(samplerData);
    return sampler;
}

void GraphicsBackendDX12::DeleteTexture(const GraphicsBackendTexture& texture)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(texture.Texture);
    resourceData->Resource->Release();
    delete resourceData;
}

void GraphicsBackendDX12::DeleteSampler(const GraphicsBackendSampler& sampler)
{
    DX12Local::SamplerData* samplerData = reinterpret_cast<DX12Local::SamplerData*>(sampler.Sampler);
    delete samplerData;
}

void GraphicsBackendDX12::BindTexture(const GraphicsBackendResourceBindings& bindings, const GraphicsBackendTexture& texture)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    DXGI_FORMAT dxFormat = DX12Helpers::ToTextureInternalFormat(texture.Format, texture.IsLinear);
    switch (dxFormat)
    {
        case DXGI_FORMAT_D16_UNORM:
            dxFormat = DXGI_FORMAT_R16_UNORM;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            dxFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            dxFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            dxFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            break;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
    resourceViewDesc.Format = dxFormat;
    resourceViewDesc.ViewDimension = DX12Helpers::ToResourceViewDimension(texture.Type);
    resourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (resourceViewDesc.ViewDimension)
    {
        case D3D12_SRV_DIMENSION_TEXTURE1D:
            resourceViewDesc.Texture1D.MipLevels = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
            resourceViewDesc.Texture1DArray.MipLevels = -1;
            resourceViewDesc.Texture1DArray.ArraySize = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURE2D:
            resourceViewDesc.Texture2D.MipLevels = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
            resourceViewDesc.Texture2DArray.MipLevels = -1;
            resourceViewDesc.Texture2DArray.ArraySize = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
            resourceViewDesc.Texture2DMSArray.ArraySize = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURE3D:
            resourceViewDesc.Texture3D.MipLevels = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURECUBE:
            resourceViewDesc.TextureCube.MipLevels = -1;
            break;
        case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
            resourceViewDesc.TextureCubeArray.MipLevels = -1;
            resourceViewDesc.TextureCubeArray.NumCubes = -1;
            break;
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(frameData.ResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameData.ResourceDescriptorHeapIndex, DX12Local::s_ResourceDescriptorSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(frameData.ResourceDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), frameData.ResourceDescriptorHeapIndex, DX12Local::s_ResourceDescriptorSize);

    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(texture.Texture);

    D3D12_RESOURCE_STATES state;
    D3D12_SHADER_VISIBILITY visibility = DX12Local::GetShaderVisibility(bindings);
    if (visibility == D3D12_SHADER_VISIBILITY_ALL)
        state = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
    else if (visibility == D3D12_SHADER_VISIBILITY_PIXEL)
        state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    else
        state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

    if (resourceData->State != state)
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resourceData->Resource, resourceData->State, state);
        frameData.RenderCommandList->ResourceBarrier(1, &barrier);
        resourceData->State = state;
    }

    DX12Local::s_Device->CreateShaderResourceView(resourceData->Resource, &resourceViewDesc, cpuHandle);

    int rootParamIndex = DX12Local::s_CurrentShaderObject->ResourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, bindings)];
    frameData.RenderCommandList->SetGraphicsRootDescriptorTable(rootParamIndex, gpuHandle);

    ++frameData.ResourceDescriptorHeapIndex;
}

void GraphicsBackendDX12::BindSampler(const GraphicsBackendResourceBindings& bindings, const GraphicsBackendSampler& sampler)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    DX12Local::SamplerData* samplerData = reinterpret_cast<DX12Local::SamplerData*>(sampler.Sampler);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(frameData.SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameData.SamplerDescriptorHeapIndex, DX12Local::s_SamplerDescriptorSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(frameData.SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), frameData.SamplerDescriptorHeapIndex, DX12Local::s_SamplerDescriptorSize);
    ++frameData.SamplerDescriptorHeapIndex;

    DX12Local::s_Device->CreateSampler(&samplerData->SamplerDesc, cpuHandle);

    int rootParamIndex = DX12Local::s_CurrentShaderObject->ResourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, bindings)];
    frameData.RenderCommandList->SetGraphicsRootDescriptorTable(rootParamIndex, gpuHandle);
}

void GraphicsBackendDX12::GenerateMipmaps(const GraphicsBackendTexture& texture)
{
}

void GraphicsBackendDX12::UploadImagePixels(const GraphicsBackendTexture& texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void* pixelsData)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(texture.Texture);
    ID3D12Resource* dxTexture = resourceData->Resource;

    UINT64 uploadBufferSize = GetRequiredIntermediateSize(dxTexture, level, 1);
    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ID3D12Resource* textureUploadHeap;
    ThrowIfFailed(DX12Local::s_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadHeap)));

    int bytesPerRow;
    if (IsCompressedTextureFormat(texture.Format))
    {
        int blockSize = GetBlockSize(texture.Format);
        int blocksPerRow = (width + (blockSize - 1)) / blockSize;
        bytesPerRow = blocksPerRow * GetBlockBytes(texture.Format);
    }
    else
    {
        bytesPerRow = imageSize / height;
    }

    D3D12_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pData = pixelsData;
    subresourceData.RowPitch = bytesPerRow;
    subresourceData.SlicePitch = imageSize;

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    UINT firstSubresource = level;
    if (texture.Type == TextureType::TEXTURE_CUBEMAP || texture.Type == TextureType::TEXTURE_CUBEMAP_ARRAY)
    {
        int totalSize = width << level;
        int mipLevels = std::log2(totalSize) + 1;
        firstSubresource = static_cast<UINT>(cubemapFace) * mipLevels + level;
    }

    D3D12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(dxTexture, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
    frameData.RenderCommandList->ResourceBarrier(1, &transition);

    UpdateSubresources(frameData.RenderCommandList.List, dxTexture, textureUploadHeap, 0, firstSubresource, 1, &subresourceData);

    transition = CD3DX12_RESOURCE_BARRIER::Transition(dxTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    frameData.RenderCommandList->ResourceBarrier(1, &transition);
}

void GraphicsBackendDX12::AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    const int attachmentIndex = static_cast<int>(descriptor.Attachment);
    DX12Local::RenderTargetState& state = DX12Local::s_RenderTargetStates[attachmentIndex];

    bool isDepth = IsDepthAttachment(descriptor.Attachment);
    bool isNullTarget = descriptor.Texture.Texture == 0;
    bool isEnabled = descriptor.IsBackbuffer || !isNullTarget;
    TextureInternalFormat backbufferFormat = isDepth ? DX12Local::k_SwapChainDepthFormat : DX12Local::k_SwapChainColorFormat;

    TextureInternalFormat format = descriptor.IsBackbuffer ? backbufferFormat : descriptor.Texture.Format;
    if (!isEnabled)
        format = TextureInternalFormat::INVALID;

    state.IsEnabled = isEnabled;
    state.NeedClear = descriptor.LoadAction == LoadAction::CLEAR;
    state.IsLinear = descriptor.IsBackbuffer || descriptor.Texture.IsLinear;
    state.Format = format;
    state.ResourceData = nullptr;

    if (!state.IsEnabled)
        return;

    if (isDepth)
    {
        if (descriptor.IsBackbuffer)
        {
            state.DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DX12Local::s_DepthBackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), DX12Local::s_SwapChain->GetCurrentBackBufferIndex(), DX12Local::s_DepthTargetDescriptorSize);
            state.ResourceData = &DX12Local::GetCurrentDepthBackbuffer();
        }
        else
        {
            state.DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(frameData.DepthTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameData.DepthTargetHeapIndex++, DX12Local::s_DepthTargetDescriptorSize);

            D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
            desc.Format = DX12Helpers::ToTextureInternalFormat(descriptor.Texture.Format, descriptor.Texture.IsLinear);
            desc.ViewDimension = DX12Helpers::ToDepthTargetViewDimension(descriptor.Texture.Type);

            switch (desc.ViewDimension)
            {
                case D3D12_DSV_DIMENSION_TEXTURE1D:
                    desc.Texture1D.MipSlice = descriptor.Level;
                    break;
                case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
                    desc.Texture1DArray.MipSlice = descriptor.Level;
                    desc.Texture1DArray.ArraySize = -1;
                    desc.Texture1DArray.FirstArraySlice = descriptor.Layer;
                    break;
                case D3D12_DSV_DIMENSION_TEXTURE2D:
                    desc.Texture2D.MipSlice = descriptor.Level;
                    break;
                case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
                    desc.Texture2DArray.MipSlice = descriptor.Level;
                    desc.Texture2DArray.ArraySize = -1;
                    desc.Texture2DArray.FirstArraySlice = descriptor.Layer;
                    break;
                case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
                    desc.Texture2DMSArray.ArraySize = -1;
                    desc.Texture2DMSArray.FirstArraySlice = descriptor.Layer;
                    break;
            }

            state.ResourceData = reinterpret_cast<DX12Local::ResourceData*>(descriptor.Texture.Texture);
            DX12Local::s_Device->CreateDepthStencilView(state.ResourceData->Resource, &desc, state.DescriptorHandle);
        }
    }
    else
    {
        if (descriptor.IsBackbuffer)
        {
            state.DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DX12Local::s_ColorBackbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), DX12Local::s_SwapChain->GetCurrentBackBufferIndex(), DX12Local::s_ColorTargetDescriptorSize);
            state.ResourceData = &DX12Local::GetCurrentColorBackbuffer();
        }
        else
        {
            state.DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(frameData.ColorTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameData.ColorTargetHeapIndex++, DX12Local::s_ColorTargetDescriptorSize);

            D3D12_RENDER_TARGET_VIEW_DESC desc{};
            desc.Format = DX12Helpers::ToTextureInternalFormat(descriptor.Texture.Format, descriptor.Texture.IsLinear);
            desc.ViewDimension = DX12Helpers::ToColorTargetViewDimension(descriptor.Texture.Type);

            switch (desc.ViewDimension)
            {
                case D3D12_RTV_DIMENSION_TEXTURE1D:
                    desc.Texture1D.MipSlice = descriptor.Level;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
                    desc.Texture1DArray.MipSlice = descriptor.Level;
                    desc.Texture1DArray.ArraySize = -1;
                    desc.Texture1DArray.FirstArraySlice = descriptor.Layer;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE2D:
                    desc.Texture2D.MipSlice = descriptor.Level;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
                    desc.Texture2DArray.MipSlice = descriptor.Level;
                    desc.Texture2DArray.ArraySize = -1;
                    desc.Texture2DArray.FirstArraySlice = descriptor.Layer;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
                    desc.Texture2DMSArray.ArraySize = -1;
                    desc.Texture2DMSArray.FirstArraySlice = descriptor.Layer;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE3D:
                    desc.Texture3D.MipSlice = descriptor.Level;
                    desc.Texture3D.FirstWSlice = descriptor.Layer;
                    desc.Texture3D.WSize = -1;
                    break;
            }

            state.ResourceData = reinterpret_cast<DX12Local::ResourceData*>(descriptor.Texture.Texture);
            DX12Local::s_Device->CreateRenderTargetView(state.ResourceData->Resource, &desc, state.DescriptorHandle);
        }
    }
}

TextureInternalFormat GraphicsBackendDX12::GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear)
{
    DX12Local::RenderTargetState& state = DX12Local::s_RenderTargetStates[static_cast<int>(attachment)];

    if (!state.IsEnabled && IsDepthAttachment(attachment))
    {
        for (int i = static_cast<int>(FramebufferAttachment::DEPTH_ATTACHMENT); i < static_cast<int>(FramebufferAttachment::MAX); ++i)
        {
            DX12Local::RenderTargetState& depthState = DX12Local::s_RenderTargetStates[i];
            if (depthState.IsEnabled)
            {
                state = depthState;
                break;
            }
        }
    }

    if (outIsLinear)
        *outIsLinear = state.IsLinear;
    return state.Format;
}

GraphicsBackendBuffer GraphicsBackendDX12::CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data)
{
    ID3D12Resource* dxBuffer;

    D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_GENERIC_READ;
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ThrowIfFailed(DX12Local::s_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, state, nullptr, IID_PPV_ARGS(&dxBuffer)));

    dxBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());

    if (data)
    {
        CD3DX12_RANGE readRange(0, 0);

        UINT8 *bufferData;
        ThrowIfFailed(dxBuffer->Map(0, &readRange, reinterpret_cast<void **>(&bufferData)));
        memcpy(bufferData, data, size);
        dxBuffer->Unmap(0, nullptr);
    }

    DX12Local::ResourceData* resourceData = new DX12Local::ResourceData();
    resourceData->Resource = dxBuffer;
    resourceData->State = state;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(resourceData);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendDX12::DeleteBuffer(const GraphicsBackendBuffer& buffer)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(buffer.Buffer);
    resourceData->Resource->Release();
    delete resourceData;
}

void GraphicsBackendDX12::BindBuffer(const GraphicsBackendBuffer& buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(buffer.Buffer);

    int rootParamIndex = DX12Local::s_CurrentShaderObject->ResourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, bindings)];
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    frameData.RenderCommandList->SetGraphicsRootShaderResourceView(rootParamIndex, resourceData->Resource->GetGPUVirtualAddress() + offset);
}

void GraphicsBackendDX12::BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(buffer.Buffer);

    int rootParamIndex = DX12Local::s_CurrentShaderObject->ResourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, bindings)];
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    frameData.RenderCommandList->SetGraphicsRootConstantBufferView(rootParamIndex, resourceData->Resource->GetGPUVirtualAddress() + offset);
}

void GraphicsBackendDX12::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void* data)
{
    DX12Local::ResourceData* resourceData = reinterpret_cast<DX12Local::ResourceData*>(buffer.Buffer);
    ID3D12Resource* dxBuffer = resourceData->Resource;

    CD3DX12_RANGE readRange(0, 0);
    CD3DX12_RANGE writeRange(offset, offset + size);

    UINT8* bufferData;
    ThrowIfFailed(dxBuffer->Map(0, &readRange, reinterpret_cast<void **>(&bufferData)));
    memcpy(bufferData + offset, data, size);
    dxBuffer->Unmap(0, &writeRange);
}

void GraphicsBackendDX12::CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size)
{
}

uint64_t GraphicsBackendDX12::GetMaxConstantBufferSize()
{
    return 0;
}

int GraphicsBackendDX12::GetConstantBufferOffsetAlignment()
{
    return 256;
}

GraphicsBackendGeometry GraphicsBackendDX12::CreateGeometry(const GraphicsBackendBuffer& vertexBuffer, const GraphicsBackendBuffer& indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes, const std::string& name)
{
    DX12Local::ResourceData* vertexResourceData = reinterpret_cast<DX12Local::ResourceData*>(vertexBuffer.Buffer);
    DX12Local::ResourceData* indexResourceData = reinterpret_cast<DX12Local::ResourceData*>(indexBuffer.Buffer);

    ID3D12Resource* dxVertexBuffer = reinterpret_cast<ID3D12Resource*>(vertexResourceData->Resource);
    ID3D12Resource* dxIndexBuffer = reinterpret_cast<ID3D12Resource*>(indexResourceData->Resource);

    D3D12_VERTEX_BUFFER_VIEW* vertexBufferView = new D3D12_VERTEX_BUFFER_VIEW;
    vertexBufferView->BufferLocation = dxVertexBuffer->GetGPUVirtualAddress();
    vertexBufferView->SizeInBytes = vertexBuffer.Size;
    vertexBufferView->StrideInBytes = vertexAttributes[0].Stride;

    D3D12_INDEX_BUFFER_VIEW* indexBufferView = nullptr;
    if (dxIndexBuffer)
    {
        indexBufferView = new D3D12_INDEX_BUFFER_VIEW;
        indexBufferView->BufferLocation = dxIndexBuffer->GetGPUVirtualAddress();
        indexBufferView->SizeInBytes = indexBuffer.Size;
        indexBufferView->Format = DXGI_FORMAT_R32_UINT;
    }

    DX12Local::GeometryData* geometryData = new DX12Local::GeometryData;
    geometryData->VertexBufferView = vertexBufferView;
    geometryData->IndexBufferView = indexBufferView;

    GraphicsBackendGeometry geometry;
    geometry.VertexArrayObject = reinterpret_cast<uint64_t>(geometryData);
    return geometry;
}

void GraphicsBackendDX12::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
    DX12Local::GeometryData* geometryData = reinterpret_cast<DX12Local::GeometryData*>(geometry.VertexArrayObject);
    delete geometryData->VertexBufferView;
    delete geometryData->IndexBufferView;
    delete geometryData;
}

void GraphicsBackendDX12::SetViewport(int x, int y, int width, int height, float depthNear, float depthFar)
{
    DX12Local::s_CurrentViewport = CD3DX12_VIEWPORT(x, y, width, height, depthNear, depthFar);
}

void GraphicsBackendDX12::SetScissorRect(int x, int y, int width, int height)
{
    DX12Local::s_CurrentScissorsRect = CD3DX12_RECT(x, y, width, height);
}

GraphicsBackendShaderObject GraphicsBackendDX12::CompileShader(ShaderType shaderType, const std::string& source, const std::string& name)
{
    ID3DBlob* blob;

    bool isVertex = shaderType == ShaderType::VERTEX_SHADER;
    LPCSTR entryPoint = isVertex ? "vertexMain" : "fragmentMain";
    LPCSTR target = isVertex ? "vs_6_0" : "ps_6_0";
    D3DCompile2(source.c_str(), source.size(), name.c_str(), nullptr, nullptr, entryPoint, target, 0, 0, 0, nullptr, 0, &blob, nullptr);

    GraphicsBackendShaderObject shader{};
    shader.ShaderObject = reinterpret_cast<uint64_t>(blob);
    return shader;
}

GraphicsBackendShaderObject GraphicsBackendDX12::CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t> &shaderBinary, const std::string &name)
{
    ID3DBlob* blob;

    D3DCreateBlob(shaderBinary.size(), &blob);
    memcpy(blob->GetBufferPointer(), shaderBinary.data(), shaderBinary.size());

    GraphicsBackendShaderObject shader{};
    shader.ShaderObject = reinterpret_cast<uint64_t>(blob);
    return shader;
}

GraphicsBackendProgram GraphicsBackendDX12::CreateProgram(const GraphicsBackendProgramDescriptor& descriptor)
{
    const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes = *descriptor.VertexAttributes;

    std::vector<D3D12_INPUT_ELEMENT_DESC> dxVertexAttributes;
    for (const auto& attr: vertexAttributes)
    {
        D3D12_INPUT_ELEMENT_DESC desc{};
        desc.SemanticName = DX12Helpers::ToSemanticName(attr.Semantic);
        desc.SemanticIndex = 0;
        desc.Format = DX12Helpers::ToVertexAttributeDataType(attr.DataType, attr.Dimensions, attr.IsNormalized);
        desc.InputSlot = 0;
        desc.AlignedByteOffset = attr.Offset;
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        dxVertexAttributes.push_back(desc);
    }

    std::unordered_map<size_t, int> resourceBindingHashToRootParameterIndex;

    const std::unordered_map<std::string, GraphicsBackendTextureInfo>& textures = *descriptor.Textures;
    const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& buffers = *descriptor.Buffers;
    const std::unordered_map<std::string, GraphicsBackendSamplerInfo>& samplers = *descriptor.Samplers;

    std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
    rootParameters.reserve(textures.size() + buffers.size() + samplers.size());

    for (const auto& pair: buffers)
    {
        const std::shared_ptr<GraphicsBackendBufferInfo>& buffer = pair.second;
        const GraphicsBackendResourceBindings& bindings = buffer->GetBinding();
        const bool isConstant = buffer->GetSize() > 0;

        CD3DX12_ROOT_PARAMETER1 parameter{};
        if (isConstant)
            parameter.InitAsConstantBufferView(DX12Local::GetShaderRegister(bindings), 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, DX12Local::GetShaderVisibility(bindings));
        else
            parameter.InitAsShaderResourceView(DX12Local::GetShaderRegister(bindings), 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, DX12Local::GetShaderVisibility(bindings));
        rootParameters.push_back(parameter);

        resourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(isConstant ? D3D12_ROOT_PARAMETER_TYPE_CBV : D3D12_ROOT_PARAMETER_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, bindings)] = rootParameters.size() - 1;
    }

    for (const auto& pair: textures)
    {
        const GraphicsBackendTextureInfo& texture = pair.second;
        const GraphicsBackendResourceBindings& bindings = texture.TextureBindings;

        CD3DX12_DESCRIPTOR_RANGE1* range = new CD3DX12_DESCRIPTOR_RANGE1();
        range->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, DX12Local::GetShaderRegister(bindings));

        CD3DX12_ROOT_PARAMETER1 parameter{};
        parameter.InitAsDescriptorTable(1, range, DX12Local::GetShaderVisibility(bindings));
        rootParameters.push_back(parameter);

        resourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, bindings)] = rootParameters.size() - 1;
    }

    for (const auto& pair: samplers)
    {
        const GraphicsBackendSamplerInfo& sampler = pair.second;
        const GraphicsBackendResourceBindings& bindings = sampler.Bindings;

        CD3DX12_DESCRIPTOR_RANGE1* range = new CD3DX12_DESCRIPTOR_RANGE1();
        range->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, DX12Local::GetShaderRegister(bindings));

        CD3DX12_ROOT_PARAMETER1 parameter{};
        parameter.InitAsDescriptorTable(1, range, DX12Local::GetShaderVisibility(bindings));
        rootParameters.push_back(parameter);

        resourceBindingHashToRootParameterIndex[DX12Local::GetResourceBindingHash(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, bindings)] = rootParameters.size() - 1;
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDesc.Desc_1_1.NumParameters = rootParameters.size();
    rootSignatureDesc.Desc_1_1.pParameters = rootParameters.data();
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* rootSignatureBlob;
    ID3DBlob* errorBlob;
    if (!SUCCEEDED(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &rootSignatureBlob, &errorBlob)))
        throw std::runtime_error(static_cast<const char *>(errorBlob->GetBufferPointer()));

    ID3D12RootSignature* rootSignature;
    ThrowIfFailed(DX12Local::s_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

    const std::vector<GraphicsBackendShaderObject>& shaders = *descriptor.Shaders;
    ID3DBlob* vertexBlob = reinterpret_cast<ID3DBlob*>(shaders[0].ShaderObject);
    ID3DBlob* fragmentBlob = reinterpret_cast<ID3DBlob*>(shaders[1].ShaderObject);

    D3D12_BLEND_DESC blendDescriptor = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blendDescriptor.RenderTarget[0].BlendEnable = descriptor.ColorAttachmentDescriptor.BlendingEnabled;
    blendDescriptor.RenderTarget[0].SrcBlend = DX12Helpers::ToBlendFactor(descriptor.ColorAttachmentDescriptor.SourceFactor);
    blendDescriptor.RenderTarget[0].DestBlend = DX12Helpers::ToBlendFactor(descriptor.ColorAttachmentDescriptor.DestinationFactor);

    D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterizerDesc.CullMode = DX12Helpers::ToCullFace(descriptor.CullFace);
    rasterizerDesc.FrontCounterClockwise = descriptor.CullFaceOrientation == CullFaceOrientation::COUNTER_CLOCKWISE;

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depthStencilDesc.DepthEnable = descriptor.DepthWrite;
    depthStencilDesc.DepthFunc = DX12Helpers::ToDepthFunction(descriptor.DepthFunction);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = { dxVertexAttributes.data(), static_cast<UINT>(dxVertexAttributes.size()) };
    psoDesc.pRootSignature = rootSignature;
    psoDesc.VS = { reinterpret_cast<UINT8*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<UINT8*>(fragmentBlob->GetBufferPointer()), fragmentBlob->GetBufferSize() };
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.BlendState = blendDescriptor;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = DX12Helpers::ToPrimitiveTopologyType(descriptor.PrimitiveType);
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DX12Helpers::ToTextureInternalFormat(descriptor.ColorAttachmentDescriptor.Format, descriptor.ColorAttachmentDescriptor.IsLinear);
    psoDesc.DSVFormat = DX12Helpers::ToTextureInternalFormat(descriptor.DepthFormat, true);
    psoDesc.SampleDesc.Count = 1;

    ID3D12PipelineState* pso;
    ThrowIfFailed(DX12Local::s_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

    if (descriptor.Name && !descriptor.Name->empty())
    {
        std::string psoName = (*descriptor.Name) + "_PSO";
        std::string rootSignatureName = (*descriptor.Name) + "_RootSignature";
        pso->SetPrivateData(WKPDID_D3DDebugObjectName, psoName.size(), psoName.c_str());
        rootSignature->SetPrivateData(WKPDID_D3DDebugObjectName, rootSignatureName.size(), rootSignatureName.c_str());
    }

    DX12Local::ShaderObject* shaderObject = new DX12Local::ShaderObject();
    shaderObject->PSO = pso;
    shaderObject->RootSignature = rootSignature;
    shaderObject->ResourceBindingHashToRootParameterIndex = std::move(resourceBindingHashToRootParameterIndex);

    GraphicsBackendProgram program{};
    program.Program = reinterpret_cast<uint64_t>(shaderObject);
    return program;
}

void GraphicsBackendDX12::DeleteShader(GraphicsBackendShaderObject shader)
{
    ID3DBlob* blob = reinterpret_cast<ID3DBlob*>(shader.ShaderObject);
    blob->Release();
}

void GraphicsBackendDX12::DeleteProgram(GraphicsBackendProgram program)
{
    DX12Local::ShaderObject* shaderObject = reinterpret_cast<DX12Local::ShaderObject*>(program.Program);
    shaderObject->PSO->Release();
    shaderObject->RootSignature->Release();
}

bool GraphicsBackendDX12::RequireStrictPSODescriptor()
{
    return true;
}

void GraphicsBackendDX12::UseProgram(GraphicsBackendProgram program)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    DX12Local::ShaderObject* shaderObject = reinterpret_cast<DX12Local::ShaderObject*>(program.Program);
    frameData.RenderCommandList->SetGraphicsRootSignature(shaderObject->RootSignature);
    frameData.RenderCommandList->SetPipelineState(shaderObject->PSO);

    DX12Local::s_CurrentShaderObject = shaderObject;
}

void GraphicsBackendDX12::SetClearColor(float r, float g, float b, float a)
{
    DX12Local::s_ClearColor[0] = r;
    DX12Local::s_ClearColor[1] = g;
    DX12Local::s_ClearColor[2] = b;
    DX12Local::s_ClearColor[3] = a;
}

void GraphicsBackendDX12::SetClearDepth(double depth)
{
    DX12Local::s_ClearDepth = depth;
}

void GraphicsBackendDX12::DrawArrays(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    DrawArraysInstanced(geometry, primitiveType, firstIndex, count, 1);
}

void GraphicsBackendDX12::DrawArraysInstanced(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    DX12Local::GeometryData* geometryData = reinterpret_cast<DX12Local::GeometryData*>(geometry.VertexArrayObject);

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    frameData.RenderCommandList->RSSetViewports(1, &DX12Local::s_CurrentViewport);
    frameData.RenderCommandList->RSSetScissorRects(1, &DX12Local::s_CurrentScissorsRect);
    frameData.RenderCommandList->IASetPrimitiveTopology(DX12Helpers::ToPrimitiveTopology(primitiveType));
    frameData.RenderCommandList->IASetVertexBuffers(0, 1, geometryData->VertexBufferView);
    frameData.RenderCommandList->DrawInstanced(indicesCount, instanceCount, firstIndex, 0);
}

void GraphicsBackendDX12::DrawElements(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    DrawElementsInstanced(geometry, primitiveType, elementsCount, dataType, 1);
}

void GraphicsBackendDX12::DrawElementsInstanced(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    DX12Local::GeometryData* geometryData = reinterpret_cast<DX12Local::GeometryData*>(geometry.VertexArrayObject);

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    frameData.RenderCommandList->RSSetViewports(1, &DX12Local::s_CurrentViewport);
    frameData.RenderCommandList->RSSetScissorRects(1, &DX12Local::s_CurrentScissorsRect);
    frameData.RenderCommandList->IASetPrimitiveTopology(DX12Helpers::ToPrimitiveTopology(primitiveType));
    frameData.RenderCommandList->IASetVertexBuffers(0, 1, geometryData->VertexBufferView);
    frameData.RenderCommandList->IASetIndexBuffer(geometryData->IndexBufferView);
    frameData.RenderCommandList->DrawIndexedInstanced(elementsCount, instanceCount, 0, 0, 0);
}

void GraphicsBackendDX12::CopyTextureToTexture(const GraphicsBackendTexture& source, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
}

void GraphicsBackendDX12::PushDebugGroup(const std::string& name, GPUQueue queue)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    PIXBeginEvent(DX12Local::GetCommandList(frameData, queue), 0, name.c_str());
}

void GraphicsBackendDX12::PopDebugGroup(GPUQueue queue)
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    PIXEndEvent(DX12Local::GetCommandList(frameData, queue));
}

GraphicsBackendProfilerMarker GraphicsBackendDX12::PushProfilerMarker()
{
    return {};
}

void GraphicsBackendDX12::PopProfilerMarker(GraphicsBackendProfilerMarker& marker)
{
}

bool GraphicsBackendDX12::ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResults& outResults)
{
    return true;
}

void GraphicsBackendDX12::BeginRenderPass(const std::string& name)
{
    PushDebugGroup(name, GPUQueue::RENDER);

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    static std::vector<bool> colorTargetNeedsClear;
    static std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> colorTargetHandles;
    static std::vector<D3D12_RESOURCE_BARRIER> transitions;

    bool hasDepthTarget = false;
    bool depthTargetNeedsClear = false;
    D3D12_CPU_DESCRIPTOR_HANDLE depthTargetHandle;

    colorTargetNeedsClear.clear();
    colorTargetHandles.clear();
    transitions.clear();

    for (int i = 0; i < static_cast<int>(FramebufferAttachment::MAX); ++i)
    {
        const DX12Local::RenderTargetState& state = DX12Local::s_RenderTargetStates[i];
        if (!state.IsEnabled)
            continue;

        const FramebufferAttachment attachment = static_cast<FramebufferAttachment>(i);

        D3D12_RESOURCE_STATES resourceState;
        if (IsDepthAttachment(attachment))
        {
            hasDepthTarget = true;
            depthTargetHandle = state.DescriptorHandle;
            depthTargetNeedsClear = state.NeedClear;
            resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }
        else
        {
            colorTargetHandles.push_back(state.DescriptorHandle);
            colorTargetNeedsClear.push_back(state.NeedClear);
            resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        }

        if (state.ResourceData && state.ResourceData->State != resourceState)
        {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(state.ResourceData->Resource, state.ResourceData->State, resourceState);
            transitions.push_back(barrier);
            state.ResourceData->State = resourceState;
        }
    }

    ID3D12DescriptorHeap* heaps[] = {frameData.ResourceDescriptorHeap, frameData.SamplerDescriptorHeap};
    frameData.RenderCommandList->SetDescriptorHeaps(2, heaps);

    if (!transitions.empty())
        frameData.RenderCommandList->ResourceBarrier(transitions.size(), &transitions[0]);

    frameData.RenderCommandList->OMSetRenderTargets(colorTargetHandles.size(), colorTargetHandles.empty() ? nullptr : &colorTargetHandles[0], false, hasDepthTarget ? &depthTargetHandle : nullptr);

    for (int i = 0; i < colorTargetHandles.size(); ++i)
    {
        if (colorTargetNeedsClear[i])
            frameData.RenderCommandList->ClearRenderTargetView(colorTargetHandles[i], DX12Local::s_ClearColor, 0, nullptr);
    }

    if (hasDepthTarget && depthTargetNeedsClear)
        frameData.RenderCommandList->ClearDepthStencilView(depthTargetHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, DX12Local::s_ClearDepth, 0, 0, nullptr);
}

void GraphicsBackendDX12::EndRenderPass()
{
    DX12Local::ResetRenderTargetStates();
    PopDebugGroup(GPUQueue::RENDER);

    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    frameData.RenderCommandList.Execute();
}

void GraphicsBackendDX12::BeginCopyPass(const std::string& name)
{
    PushDebugGroup(name, GPUQueue::COPY);
}

void GraphicsBackendDX12::EndCopyPass()
{
    PopDebugGroup(GPUQueue::COPY);
}

GraphicsBackendFence GraphicsBackendDX12::CreateFence(FenceType fenceType, const std::string& name)
{
    return {};
}

void GraphicsBackendDX12::DeleteFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendDX12::SignalFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendDX12::WaitForFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendDX12::Flush()
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();
    frameData.RenderCommandList.Execute();
    frameData.CopyCommandList.Execute();
}

void GraphicsBackendDX12::Present()
{
    DX12Local::PerFrameData& frameData = DX12Local::GetCurrentFrameData();

    CD3DX12_RESOURCE_BARRIER backbufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(DX12Local::GetCurrentColorBackbuffer().Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    frameData.RenderCommandList->ResourceBarrier(1, &backbufferBarrier);

    Flush();
    frameData.RenderCommandList.Close();
    frameData.CopyCommandList.Close();

    DX12Local::s_SwapChain->Present(1, 0);
    DX12Local::PrintInfoQueueMessages();

    uint64_t fenceValue = frameData.FenceValue++;
    ThrowIfFailed(DX12Local::s_RenderQueue->Signal(frameData.RenderQueueFence, fenceValue));
    ThrowIfFailed(DX12Local::s_CopyQueue->Signal(frameData.CopyQueueFence, fenceValue));
}

#undef ThrowIfFailed

#endif // RENDER_BACKEND_DX12