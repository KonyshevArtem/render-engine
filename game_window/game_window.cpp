#include "game_window.h"

#ifdef ENABLE_IMGUI
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"
#endif

#include <utility>

GameWindow::GameWindow(RenderHandler renderHandler, KeyboardInputHandlerDelegate keyboardInputHandler, MouseMoveHandlerDelegate mouseMoveHandler):
    m_RenderHandler(std::move(renderHandler)),
    m_KeyboardInputHandler(std::move(keyboardInputHandler)),
    m_MouseMoveHandler(std::move(mouseMoveHandler))
{
#ifdef ENABLE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
#endif
}

GameWindow::~GameWindow()
{
#ifdef ENABLE_IMGUI
    ImGui::DestroyContext();
#endif
}

void GameWindow::TickMainLoop(int width, int height)
{
#ifdef ENABLE_IMGUI
    ImGui::NewFrame();
#endif

    if (m_RenderHandler)
    {
        m_RenderHandler(width, height);
    }

#ifdef ENABLE_IMGUI
    TopMenuBar::Draw([this](){ m_CloseFlag = true; });
    WindowManager::DrawAllWindows();

    ImGui::Render();
#endif
}

void GameWindow::ProcessKeyPress(char key, bool pressed)
{
#ifdef ENABLE_IMGUI
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
#endif
    {
        if (m_KeyboardInputHandler)
        {
            m_KeyboardInputHandler(key, pressed);
        }
    }
}

void GameWindow::ProcessMouseMove(float x, float y)
{
#ifdef ENABLE_IMGUI
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
#endif
    {
        if (m_MouseMoveHandler)
        {
            m_MouseMoveHandler(x, y);
        }
    }
}

bool GameWindow::ShouldCloseWindow() const
{
    return m_CloseFlag;
}