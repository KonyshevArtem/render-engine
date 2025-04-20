#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_dx12.h"

#include <windows.h>
#include <string>
#include <d3d12.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGuiWrapper
{
    enum GraphicsBackend
    {
        OPENGL,
        DX12
    };

    GraphicsBackend s_Backend;

    void Init(const std::string& graphicsBackend, const std::function<void(void*)>& fillImGuiData)
    {
        struct InitDataOpenGL
        {
            void* Window;
            int OpenGLMajorVersion;
            int OpenGLMinorVersion;
        };

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

        if (graphicsBackend == "OpenGL")
            s_Backend = OPENGL;
        else if (graphicsBackend == "DX12")
            s_Backend = DX12;

        ImGuiWrapperCommon::Init();

        if (s_Backend == OPENGL)
        {
            InitDataOpenGL data;
            fillImGuiData(reinterpret_cast<void*>(&data));

            std::string glslVersion = "#version " + std::to_string(data.OpenGLMajorVersion * 100 + data.OpenGLMinorVersion * 10);

            ImGui_ImplWin32_InitForOpenGL(data.Window);
            ImGui_ImplOpenGL3_Init(glslVersion.c_str());
        }
        else if (s_Backend == DX12)
        {
            InitDataDX12 data;
            fillImGuiData(reinterpret_cast<void*>(&data));

            ImGui_ImplWin32_Init(data.Window);
            ImGui_ImplDX12_Init(data.Device, data.MaxFramesInFlight, data.Format, data.DescriptorHeap, data.CpuDescriptorHandle, data.GpuDescriptorHandle);
        }
    }

    void Shutdown()
    {
        if (s_Backend == OPENGL)
            ImGui_ImplOpenGL3_Shutdown();
        else if (s_Backend == DX12)
            ImGui_ImplDX12_Shutdown();

        ImGui_ImplWin32_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame()
    {
        if (s_Backend == OPENGL)
            ImGui_ImplOpenGL3_NewFrame();
        else if (s_Backend == DX12)
            ImGui_ImplDX12_NewFrame();

        ImGui_ImplWin32_NewFrame();
        ImGuiWrapperCommon::NewFrame();
    }

    void Render(const std::function<void(void*)>& fillImGuiData)
    {
        struct FrameDataDX12
        {
            ID3D12GraphicsCommandList* CommandList;
        };

        ImGuiWrapperCommon::Render();

        if (s_Backend == OPENGL)
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        else if (s_Backend == DX12)
        {
            FrameDataDX12 data;

            fillImGuiData(reinterpret_cast<void*>(&data));
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), data.CommandList);
        }
    }

    void ProcessMessage(void* data)
    {
        struct Data
        {
            HWND Window;
            UINT Message;
            WPARAM WParam;
            LPARAM LParam;
        };

        Data* messageData = static_cast<Data*>(data);
        ImGui_ImplWin32_WndProcHandler(messageData->Window, messageData->Message, messageData->WParam, messageData->LParam);
    }
}

#else

namespace ImGuiWrapper
{
    void Init(const std::string& graphicsBackend, const std::function<void(void*)>& fillImGuiData) {}
    void Shutdown() {}
    void NewFrame() {}
    void Render(const std::function<void(void*)>& fillImGuiData) {}
    void ProcessMessage(void* data) {}
}

#endif