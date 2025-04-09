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
#include "types/graphics_backend_depth_stencil_state.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "types/graphics_backend_profiler_marker.h"
#include "types/graphics_backend_sampler_info.h"
#include "helpers/dx12_helpers.h"
#include "debug.h"

IDXGISwapChain4* s_SwapChain;

ID3D12Device* s_Device;
ID3D12CommandQueue* s_RenderQueue;
ID3D12CommandQueue* s_CopyQueue;
ID3D12CommandAllocator* s_RenderCommandAllocator;
ID3D12CommandAllocator* s_CopyCommandAllocator;

ID3D12DescriptorHeap* s_RenderTargetDescriptorHeap;
UINT s_RenderTargetDescriptorSize;

ID3D12Resource* s_RenderTargets[GraphicsBackend::GetMaxFramesInFlight()];

struct DX12ShaderObject
{
    ID3D12PipelineState* PSO;
    ID3D12RootSignature* RootSignature;
};

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

void GraphicsBackendDX12::Init(void* data)
{
    HWND* window = static_cast<HWND*>(data);

    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }

    IDXGIFactory7* factory;
    CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));

    IDXGIAdapter4* adapter;
    GetHardwareAdapter(factory, &adapter);

    D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&s_Device));

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    s_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&s_RenderQueue));

    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    s_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&s_CopyQueue));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.BufferCount = GraphicsBackend::GetMaxFramesInFlight();
    swapChainDesc.Width = 800; // Match the window's client area
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGISwapChain1* swapChain;
    factory->CreateSwapChainForHwnd(
            s_RenderQueue,
            *window,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
    );
    swapChain->QueryInterface(__uuidof(IDXGISwapChain4), reinterpret_cast<void**>(&s_SwapChain));
    swapChain->Release();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = GraphicsBackend::GetMaxFramesInFlight();
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    s_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&s_RenderTargetDescriptorHeap));
    s_RenderTargetDescriptorSize = s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(s_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < GraphicsBackend::GetMaxFramesInFlight(); n++)
    {
        s_SwapChain->GetBuffer(n, IID_PPV_ARGS(&s_RenderTargets[n]));
        s_Device->CreateRenderTargetView(s_RenderTargets[n], nullptr, rtvHandle);
        rtvHandle.ptr += s_RenderTargetDescriptorSize;
    }

    s_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&s_RenderCommandAllocator));
    s_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&s_CopyCommandAllocator));
}

GraphicsBackendName GraphicsBackendDX12::GetName()
{
    return GraphicsBackendName::DX12;
}

void GraphicsBackendDX12::InitNewFrame()
{
    GraphicsBackendBase::InitNewFrame();
}

void GraphicsBackendDX12::FillImGuiData(void* data)
{
}

GraphicsBackendTexture GraphicsBackendDX12::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name)
{
    return {};
}

GraphicsBackendSampler GraphicsBackendDX12::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, const std::string& name)
{
    return {};
}

void GraphicsBackendDX12::DeleteTexture(const GraphicsBackendTexture& texture)
{
}

void GraphicsBackendDX12::DeleteSampler(const GraphicsBackendSampler& sampler)
{
}

void GraphicsBackendDX12::BindTexture(const GraphicsBackendResourceBindings& bindings, const GraphicsBackendTexture& texture)
{
}

void GraphicsBackendDX12::BindSampler(const GraphicsBackendResourceBindings& bindings, const GraphicsBackendSampler& sampler)
{
}

void GraphicsBackendDX12::GenerateMipmaps(const GraphicsBackendTexture& texture)
{
}

void GraphicsBackendDX12::UploadImagePixels(const GraphicsBackendTexture& texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void* pixelsData)
{
}

void GraphicsBackendDX12::AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor)
{
}

TextureInternalFormat GraphicsBackendDX12::GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear)
{
    return TextureInternalFormat::RGBA8;
}

GraphicsBackendBuffer GraphicsBackendDX12::CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data)
{
    ID3D12Resource* dxBuffer;
//    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

    D3D12_HEAP_PROPERTIES heapProps;
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ThrowIfFailed(DX12Local::s_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&dxBuffer)));

    if (data)
    {
        D3D12_RANGE readRange;
        readRange.Begin = 0;
        readRange.End = 0;

        UINT8 *bufferData;
        ThrowIfFailed(dxBuffer->Map(0, &readRange, reinterpret_cast<void **>(&bufferData)));
        memcpy(bufferData, data, size);
        dxBuffer->Unmap(0, nullptr);
    }

//    vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
//    vertexBufferView.StrideInBytes = sizeof(Vertex);
//    vertexBufferView.SizeInBytes = vertexBufferSize;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(dxBuffer);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendDX12::DeleteBuffer(const GraphicsBackendBuffer& buffer)
{
}

void GraphicsBackendDX12::BindBuffer(const GraphicsBackendBuffer& buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
}

void GraphicsBackendDX12::BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
}

void GraphicsBackendDX12::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void* data)
{
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
    return {};
}

void GraphicsBackendDX12::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
}

void GraphicsBackendDX12::SetCullFace(CullFace cullFace)
{
}

void GraphicsBackendDX12::SetCullFaceOrientation(CullFaceOrientation orientation)
{
}

void GraphicsBackendDX12::SetViewport(int x, int y, int width, int height, float near, float far)
{
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

GraphicsBackendProgram GraphicsBackendDX12::CreateProgram(const std::vector<GraphicsBackendShaderObject>& shaders, const GraphicsBackendColorAttachmentDescriptor& colorAttachmentDescriptor, TextureInternalFormat depthFormat,
                                                            const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes,
                                                            std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
                                                            std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
                                                            std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
                                                            const std::string& name)
{
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

    auto GetShaderRegister = [](const GraphicsBackendResourceBindings& bindings) -> UINT
    {
        return bindings.VertexIndex != -1 ? bindings.VertexIndex : bindings.FragmentIndex;
    };

    auto GetShaderVisibility = [](const GraphicsBackendResourceBindings& bindings) -> D3D12_SHADER_VISIBILITY
    {
        const bool isVertex = bindings.VertexIndex != -1;
        const bool isFragment = bindings.FragmentIndex != -1;

        if (isVertex && isFragment)
            return D3D12_SHADER_VISIBILITY_ALL;
        return isVertex ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL;
    };

    std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
    rootParameters.reserve(textures.size() + buffers.size() + samplers.size());

    for (const auto& pair: buffers)
    {
        const std::shared_ptr<GraphicsBackendBufferInfo>& buffer = pair.second;
        const GraphicsBackendResourceBindings& bindings = buffer->GetBinding();
        const bool isConstant = buffer->GetSize() > 0;

        D3D12_ROOT_PARAMETER1 parameter{};
        parameter.ParameterType = isConstant ? D3D12_ROOT_PARAMETER_TYPE_CBV : D3D12_ROOT_PARAMETER_TYPE_SRV;
        parameter.Descriptor.ShaderRegister = GetShaderRegister(bindings);
        parameter.ShaderVisibility = GetShaderVisibility(bindings);
        rootParameters.push_back(parameter);
    }

    for (const auto& pair: textures)
    {
        const GraphicsBackendTextureInfo& texture = pair.second;
        const GraphicsBackendResourceBindings& bindings = texture.TextureBindings;

        D3D12_DESCRIPTOR_RANGE1* range = new D3D12_DESCRIPTOR_RANGE1();
        range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range->BaseShaderRegister = GetShaderRegister(bindings);
        range->NumDescriptors = 1;

        D3D12_ROOT_PARAMETER1 parameter{};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.pDescriptorRanges = range;
        parameter.DescriptorTable.NumDescriptorRanges = 1;
        rootParameters.push_back(parameter);
    }

    for (const auto& pair: samplers)
    {
        const GraphicsBackendSamplerInfo& sampler = pair.second;
        const GraphicsBackendResourceBindings& bindings = sampler.Bindings;

        D3D12_DESCRIPTOR_RANGE1* range = new D3D12_DESCRIPTOR_RANGE1();
        range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        range->BaseShaderRegister = GetShaderRegister(bindings);
        range->NumDescriptors = 1;

        D3D12_ROOT_PARAMETER1 parameter{};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.pDescriptorRanges = range;
        parameter.DescriptorTable.NumDescriptorRanges = 1;
        rootParameters.push_back(parameter);
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
    s_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    ID3DBlob* vertexBlob = reinterpret_cast<ID3DBlob*>(shaders[0].ShaderObject);
    ID3DBlob* fragmentBlob = reinterpret_cast<ID3DBlob*>(shaders[1].ShaderObject);

    D3D12_BLEND_DESC blendDescriptor = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    blendDescriptor.RenderTarget[0].BlendEnable = colorAttachmentDescriptor.BlendingEnabled;
    blendDescriptor.RenderTarget[0].SrcBlend = DX12Helpers::ToBlendFactor(colorAttachmentDescriptor.SourceFactor);
    blendDescriptor.RenderTarget[0].DestBlend = DX12Helpers::ToBlendFactor(colorAttachmentDescriptor.DestinationFactor);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = { dxVertexAttributes.data(), static_cast<UINT>(dxVertexAttributes.size()) };
    psoDesc.pRootSignature = rootSignature;
    psoDesc.VS = { reinterpret_cast<UINT8*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<UINT8*>(fragmentBlob->GetBufferPointer()), fragmentBlob->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = blendDescriptor;
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DX12Helpers::ToTextureInternalFormat(colorAttachmentDescriptor.Format, colorAttachmentDescriptor.IsLinear);
    psoDesc.DSVFormat = DX12Helpers::ToTextureInternalFormat(depthFormat, true);
    psoDesc.SampleDesc.Count = 1;

    ID3D12PipelineState* pso;
    s_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

    DX12ShaderObject* shaderObject = new DX12ShaderObject();
    shaderObject->PSO = pso;
    shaderObject->RootSignature = rootSignature;

    GraphicsBackendProgram program{};
    program.Program = reinterpret_cast<uint64_t>(shaderObject);
    return program;
}

void GraphicsBackendDX12::DeleteShader(GraphicsBackendShaderObject shader)
{
}

void GraphicsBackendDX12::DeleteProgram(GraphicsBackendProgram program)
{
}

bool GraphicsBackendDX12::RequireStrictPSODescriptor()
{
    return true;
}

void GraphicsBackendDX12::UseProgram(GraphicsBackendProgram program)
{
}

void GraphicsBackendDX12::SetClearColor(float r, float g, float b, float a)
{
}

void GraphicsBackendDX12::SetClearDepth(double depth)
{
}

void GraphicsBackendDX12::DrawArrays(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
}

void GraphicsBackendDX12::DrawArraysInstanced(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
}

void GraphicsBackendDX12::DrawElements(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
}

void GraphicsBackendDX12::DrawElementsInstanced(const GraphicsBackendGeometry& geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
}

void GraphicsBackendDX12::CopyTextureToTexture(const GraphicsBackendTexture& source, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
}

void GraphicsBackendDX12::PushDebugGroup(const std::string& name)
{
}

void GraphicsBackendDX12::PopDebugGroup()
{
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
}

void GraphicsBackendDX12::EndRenderPass()
{
}

void GraphicsBackendDX12::BeginCopyPass(const std::string& name)
{
}

void GraphicsBackendDX12::EndCopyPass()
{
}

GraphicsBackendDepthStencilState GraphicsBackendDX12::CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction, const std::string& name)
{
    return {};
}

void GraphicsBackendDX12::DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
}

void GraphicsBackendDX12::SetDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
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
}

void GraphicsBackendDX12::Present()
{
}

#endif // RENDER_BACKEND_DX12