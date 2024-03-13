#include "top_menu_bar.h"
#include "imgui.h"
#include "editor/gizmos/gizmos.h"
#include "windows/debug_console_window.h"
#include "windows/scene_hierarchy_window.h"
#include "windows/graphics_settings_window.h"

void DrawEngineMenu(std::function<void()> &closeWindow)
{
    if (ImGui::BeginMenu("Engine"))
    {
        if (ImGui::MenuItem("Exit") && closeWindow)
        {
            closeWindow();
        }

        ImGui::EndMenu();
    }
}

void DrawDebugMenu()
{
    if (ImGui::BeginMenu("Debug"))
    {
        bool gizmosEnabled = Gizmos::IsEnabled();
        if (ImGui::MenuItem("Draw Gizmos", nullptr, gizmosEnabled))
        {
            Gizmos::SetEnabled(!gizmosEnabled);
        }

        ImGui::EndMenu();
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

        ImGui::EndMenu();
    }
}

void TopMenuBar::Draw(std::function<void()> closeWindow)
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawEngineMenu(closeWindow);
        DrawDebugMenu();
        DrawWindowsMenu();

        ImGui::EndMainMenuBar();
    }
}