#include "game_window_platform_apple.h"
#include "imgui.h"
#include "imgui_impl_osx.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_opengl3.h"
#include "graphics_backend_api.h"

GameWindowPlatformApple::GameWindowPlatformApple(void *viewPtr) : GameWindow(),
    m_ViewPtr(viewPtr)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplOSX_Init(viewPtr);
    ImGui_ImplOpenGL3_Init(GraphicsBackend::GetShadingLanguageDirective().c_str());
}

GameWindowPlatformApple::~GameWindowPlatformApple()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplOSX_Shutdown();
    ImGui::DestroyContext();
}

void GameWindowPlatformApple::TickMainLoop(int width, int height)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplOSX_NewFrame(m_ViewPtr);
    ImGui::NewFrame();

    DrawInternal(width, height);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}