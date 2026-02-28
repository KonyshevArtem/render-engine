#ifdef ENABLE_IMGUI

#include "top_menu_bar.h"
#include "imgui.h"
#include "editor/gizmos/gizmos.h"
#include "windows/debug_console_window.h"
#include "windows/scene_hierarchy_window.h"
#include "windows/graphics_settings_window.h"
#include "windows/profiler_window.h"
#include "graphics_backend_api.h"
#include "graphics/render_queue/render_queue.h"
#include "editor/debug_pass/shadow_map_debug_pass.h"

namespace TopMenuBarLocal
{
    std::string GetBackendName(GraphicsBackendName name)
    {
        switch (name)
        {
            case GraphicsBackendName::OPENGL:
                return "OpenGL";
            case GraphicsBackendName::GLES:
                return "GLES";
            case GraphicsBackendName::METAL:
                return "Metal";
            case GraphicsBackendName::DX12:
                return "DX12";
        }
    }
}

void DrawWindowsMenu()
{
    if (ImGui::BeginMenu("Windows"))
    {
        if (ImGui::MenuItem("Scene Hierarchy"))
        {
            WindowManager::Create<SceneHierarchyWindow>();
        }

        if (ImGui::MenuItem("Debug Console"))
        {
            WindowManager::Create<DebugConsoleWindow>();
        }

        if (ImGui::MenuItem("Graphics Settings"))
        {
            WindowManager::Create<GraphicsSettingsWindow>();
        }

        if (ImGui::MenuItem("Profiler"))
        {
            WindowManager::Create<ProfilerWindow>();
        }

        ImGui::EndMenu();
    }
}

void TopMenuBar::Draw()
{
    Profiler::Marker _("TopMenuBar::Draw");

    if (ImGui::BeginMainMenuBar())
    {
        const float menuWidth = ImGui::GetContentRegionAvail().x;

        DrawWindowsMenu();

        const std::string backendName = TopMenuBarLocal::GetBackendName(GraphicsBackend::Current()->GetName());
        const float textWidth = ImGui::CalcTextSize(backendName.c_str()).x;

        ImGui::SetCursorPosX(menuWidth - textWidth);
        ImGui::Text("%s", backendName.c_str());

        ImGui::EndMainMenuBar();
    }
}

#endif