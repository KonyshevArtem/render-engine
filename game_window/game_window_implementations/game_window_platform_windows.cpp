#if RENDER_ENGINE_WINDOWS

#include "game_window_platform_windows.h"
#include "imgui.h"

GameWindowPlatformWindows::GameWindowPlatformWindows() : GameWindow()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
}

GameWindowPlatformWindows::~GameWindowPlatformWindows()
{
    ImGui::DestroyContext();
}

void GameWindowPlatformWindows::TickMainLoop(int width, int height)
{
    ImGui::NewFrame();

    DrawInternal(width, height);

    ImGui::Render();
}

#endif