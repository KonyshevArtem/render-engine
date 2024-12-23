#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <string>

namespace ImGuiWrapper
{
    struct InitData
    {
        GLFWwindow* Window;
        int OpenGLMajorVersion;
        int OpenGLMinorVersion;
    };

    void Init(void* data)
    {
        InitData* initData = static_cast<InitData*>(data);

        std::string glslVersion = "#version " + std::to_string(initData->OpenGLMajorVersion * 100 + initData->OpenGLMinorVersion * 10);

        ImGuiWrapperCommon::Init();
        ImGui_ImplGlfw_InitForOpenGL(initData->Window, true);
        ImGui_ImplOpenGL3_Init(glslVersion.c_str());
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame(void* data)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGuiWrapperCommon::NewFrame();
    }

    void Render(void* data)
    {
        ImGuiWrapperCommon::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

#else

namespace ImGuiWrapper
{
    void Init(void* data) {}
    void Shutdown() {}
    void NewFrame(void* data) {}
    void Render(void* data) {}
}

#endif