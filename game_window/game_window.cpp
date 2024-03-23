#include "game_window.h"
#include "top_menu_bar.h"
#include "window_manager.h"
#include "imgui.h"

#if RENDER_ENGINE_WINDOWS
#include "game_window_implementations/game_window_platform_windows.h"
#elif RENDER_ENGINE_APPLE
#include "game_window_implementations/game_window_platform_apple.h"
#endif

#include <utility>

RenderHandler s_RenderHandler;
KeyboardInputHandlerDelegate s_KeyboardInputHandler;
MouseMoveHandlerDelegate s_MouseMoveHandler;

GameWindow *GameWindow::Create(void* viewPtr,
                               RenderHandler renderHandler,
                               KeyboardInputHandlerDelegate keyboardInputHandler,
                               MouseMoveHandlerDelegate mouseMoveHandler)
{
    s_RenderHandler = std::move(renderHandler);
    s_KeyboardInputHandler = std::move(keyboardInputHandler);
    s_MouseMoveHandler = std::move(mouseMoveHandler);

#if RENDER_ENGINE_WINDOWS
    return new GameWindowPlatformWindows(viewPtr);
#elif RENDER_ENGINE_APPLE
    return new GameWindowPlatformApple(viewPtr);
#endif
}

void GameWindow::DrawInternal(int width, int height)
{
    if (s_RenderHandler)
    {
        s_RenderHandler(width, height);
    }

    TopMenuBar::Draw([this](){ m_CloseFlag = true; });
    WindowManager::DrawAllWindows();
}

void GameWindow::ProcessKeyPress(char key, bool pressed)
{
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard && s_KeyboardInputHandler)
    {
        s_KeyboardInputHandler(key, pressed);
    }
}

void GameWindow::ProcessMouseMove(float x, float y)
{
    auto &io = ImGui::GetIO();
    if (!io.WantCaptureMouse && s_MouseMoveHandler)
    {
        s_MouseMoveHandler(x, y);
    }
}

bool GameWindow::ShouldCloseWindow() const
{
    return m_CloseFlag;
}