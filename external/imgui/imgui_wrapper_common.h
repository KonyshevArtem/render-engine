#ifndef RENDER_ENGINE_IMGUI_WRAPPER_COMMON_H
#define RENDER_ENGINE_IMGUI_WRAPPER_COMMON_H

#ifdef ENABLE_IMGUI

#include "imgui.h"

namespace ImGuiWrapperCommon
{
    void Init()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        ImGui::StyleColorsDark();
    }

    void Shutdown()
    {
        ImGui::DestroyContext();
    }

    void NewFrame()
    {
        ImGui::NewFrame();
    }

    void Render()
    {
        ImGui::Render();
    }
}

#else

namespace ImGuiWrapperCommon
{
    void Init(){}
    void Shutdown(){}
    void NewFrame(){}
    void Render(){}
}

#endif // ENABLE_IMGUI

#endif // RENDER_ENGINE_IMGUI_WRAPPER_COMMON_H