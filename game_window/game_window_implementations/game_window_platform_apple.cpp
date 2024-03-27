#if RENDER_ENGINE_APPLE

#include "game_window_platform_apple.h"
#include "imgui.h"

GameWindowPlatformApple::GameWindowPlatformApple() : GameWindow()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
}

GameWindowPlatformApple::~GameWindowPlatformApple()
{
    ImGui::DestroyContext();
}

void GameWindowPlatformApple::TickMainLoop(int width, int height)
{
    ImGui::NewFrame();

    DrawInternal(width, height);

    ImGui::Render();
}

#endif