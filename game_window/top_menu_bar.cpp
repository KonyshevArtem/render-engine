#include "top_menu_bar.h"
#include "imgui.h"
#include "editor/gizmos/gizmos.h"
#include "windows/debug_console_window.h"

void DrawEngineMenu()
{
    if (ImGui::BeginMenu("Engine"))
    {
        if (ImGui::MenuItem("Exit"))
        {
            exit(0);
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
        if (ImGui::MenuItem("Debug Console"))
        {
            BaseWindow::Create<DebugConsoleWindow>();
        }

        ImGui::EndMenu();
    }
}

void TopMenuBar::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawEngineMenu();
        DrawDebugMenu();
        DrawWindowsMenu();

        ImGui::EndMainMenuBar();
    }
}