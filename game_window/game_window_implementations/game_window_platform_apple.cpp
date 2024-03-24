#if RENDER_ENGINE_APPLE

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

    ImGui_ImplOSX_Init(m_ViewPtr);
    switch (GraphicsBackend::Current()->GetName())
    {
        case GraphicsBackendName::OPENGL:
            ImGui_ImplOpenGL3_Init(GraphicsBackend::Current()->GetShadingLanguageDirective().c_str());
            break;
        case GraphicsBackendName::METAL:
            ImGui_ImplMetal_Init(nullptr);
            break;
    }
}

GameWindowPlatformApple::~GameWindowPlatformApple()
{
    switch (GraphicsBackend::Current()->GetName())
    {
        case GraphicsBackendName::OPENGL:
            ImGui_ImplOpenGL3_Shutdown();
            break;
        case GraphicsBackendName::METAL:
            ImGui_ImplMetal_Shutdown();
            break;
    }

    ImGui_ImplOSX_Shutdown();
    ImGui::DestroyContext();
}

void GameWindowPlatformApple::TickMainLoop(int width, int height)
{
    switch (GraphicsBackend::Current()->GetName())
    {
        case GraphicsBackendName::OPENGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        case GraphicsBackendName::METAL:
            ImGui_ImplMetal_NewFrame(nullptr);
            break;
    }

    ImGui_ImplOSX_NewFrame(m_ViewPtr);
    ImGui::NewFrame();

    DrawInternal(width, height);

    ImGui::Render();

    switch (GraphicsBackend::Current()->GetName())
    {
        case GraphicsBackendName::OPENGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        case GraphicsBackendName::METAL:
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), nullptr, nullptr);
            break;
    }
}

#endif