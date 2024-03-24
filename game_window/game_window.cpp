#include "game_window.h"
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"
#include "game_window_implementations/game_window_platform_windows.h"
#include "game_window_implementations/game_window_platform_apple.h"

#include <utility>

GameWindow *GameWindow::Create(void* viewPtr,
                               RenderHandler renderHandler,
                               KeyboardInputHandlerDelegate keyboardInputHandler,
                               MouseMoveHandlerDelegate mouseMoveHandler)
{
#if RENDER_ENGINE_WINDOWS
    GameWindow *window = new GameWindowPlatformWindows(viewPtr);
#elif RENDER_ENGINE_APPLE
    GameWindow *window = new GameWindowPlatformApple(viewPtr);
#endif

    window->m_RenderHandler = std::move(renderHandler);
    window->m_KeyboardInputHandler = std::move(keyboardInputHandler);
    window->m_MouseMoveHandler = std::move(mouseMoveHandler);

    return window;
}

void GameWindow::DrawInternal(int width, int height)
{
    if (m_RenderHandler)
    {
        m_RenderHandler(width, height);
    }

    TopMenuBar::Draw([this](){ m_CloseFlag = true; });
    WindowManager::DrawAllWindows();
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