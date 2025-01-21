#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGuiWrapper
{
    void Init(const std::function<void(void*)>& fillImGuiData)
    {
        struct InitData
        {
            HWND* Window;
            int OpenGLMajorVersion;
            int OpenGLMinorVersion;
        };

        InitData data;
        fillImGuiData(reinterpret_cast<void*>(&data));

        std::string glslVersion = "#version " + std::to_string(data.OpenGLMajorVersion * 100 + data.OpenGLMinorVersion * 10);

        ImGuiWrapperCommon::Init();
        ImGui_ImplWin32_InitForOpenGL(data.Window);
        ImGui_ImplOpenGL3_Init(glslVersion.c_str());
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGuiWrapperCommon::NewFrame();
    }

    void Render()
    {
        ImGuiWrapperCommon::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    void Init(const std::function<void(void*)>& fillImGuiData) {}
    void Shutdown() {}
    void NewFrame() {}
    void Render() {}
    void ProcessMessage(void* data) {}
}

#endif