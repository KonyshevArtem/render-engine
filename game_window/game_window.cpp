#include "game_window.h"
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"

#include <utility>

GameWindow::GameWindow(RenderHandler renderHandler, KeyboardInputHandlerDelegate keyboardInputHandler, MouseMoveHandlerDelegate mouseMoveHandler):
    m_RenderHandler(std::move(renderHandler)),
    m_KeyboardInputHandler(std::move(keyboardInputHandler)),
    m_MouseMoveHandler(std::move(mouseMoveHandler))
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
}

GameWindow::~GameWindow()
{
    ImGui::DestroyContext();
}

void GameWindow::TickMainLoop(int width, int height)
{
    ImGui::NewFrame();

    if (m_RenderHandler)
    {
        m_RenderHandler(width, height);
    }

    TopMenuBar::Draw([this](){ m_CloseFlag = true; });
    WindowManager::DrawAllWindows();

    ImGui::Render();
}

void GameWindow::ProcessKeyPress(char key, bool pressed)
{
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard && m_KeyboardInputHandler)
    {
        m_KeyboardInputHandler(key, pressed);
    }
}

void GameWindow::ProcessMouseMove(float x, float y)
{
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureMouse && m_MouseMoveHandler)
    {
        m_MouseMoveHandler(x, y);
    }
}

bool GameWindow::ShouldCloseWindow() const
{
    return m_CloseFlag;
}