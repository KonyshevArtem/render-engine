#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <string>

namespace ImGuiWrapper
{
    void Init(const std::function<void(void*)>& fillImGuiData)
    {
        struct InitData
        {
            GLFWwindow* Window;
            int OpenGLMajorVersion;
            int OpenGLMinorVersion;
        };

        InitData data;
        fillImGuiData(reinterpret_cast<void*>(&data));

        std::string glslVersion = "#version " + std::to_string(data.OpenGLMajorVersion * 100 + data.OpenGLMinorVersion * 10);

        ImGuiWrapperCommon::Init();
        ImGui_ImplGlfw_InitForOpenGL(data.Window, true);
        ImGui_ImplOpenGL3_Init(glslVersion.c_str());
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGuiWrapperCommon::NewFrame();
    }

    void Render()
    {
        ImGuiWrapperCommon::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

#else

namespace ImGuiWrapper
{
    void Init(const std::function<void(void*)>& fillImGuiData) {}
    void Shutdown() {}
    void NewFrame() {}
    void Render() {}
}

#endif