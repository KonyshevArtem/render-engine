#include "game_window.h"

#ifdef ENABLE_IMGUI
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"
#endif

#include <utility>

GameWindow::GameWindow(RenderHandler renderHandler):
    m_RenderHandler(std::move(renderHandler))
{
}

GameWindow::~GameWindow()
{
#ifdef ENABLE_IMGUI
    WindowManager::CloseAllWindows();
#endif
}

void GameWindow::TickMainLoop(int width, int height)
{
    if (m_RenderHandler)
    {
        m_RenderHandler(width, height);
    }

#ifdef ENABLE_IMGUI
    TopMenuBar::Draw([this](){ m_CloseFlag = true; });
    WindowManager::DrawAllWindows();
#endif
}

bool GameWindow::ShouldCloseWindow() const
{
    return m_CloseFlag;
}

bool GameWindow::CaptureKeyboard() const
{
#ifdef ENABLE_IMGUI
    return ImGui::GetIO().WantCaptureKeyboard;
#else
    return false;
#endif
}

bool GameWindow::CaptureMouse() const
{
#ifdef ENABLE_IMGUI
    return ImGui::GetIO().WantCaptureMouse;
#else
    return false;
#endif
}
