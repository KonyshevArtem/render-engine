#if RENDER_ENGINE_WINDOWS

#include "game_window_platform_windows.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "graphics_backend_api.h"

GameWindowPlatformWindows::GameWindowPlatformWindows(void *viewPtr) : GameWindow()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(viewPtr), true);
    ImGui_ImplOpenGL3_Init(GraphicsBackend::Current()->GetShadingLanguageDirective().c_str());
}

GameWindowPlatformWindows::~GameWindowPlatformWindows()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GameWindowPlatformWindows::TickMainLoop(int width, int height)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawInternal(width, height);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif