#include "game_window.h"
#include "graphics_backend_api.h"
#include "top_menu_bar.h"
#include "window_manager.h"
#include "game_window_implementations/game_window_opengl.h"
#include "game_window_implementations/game_window_metal.h"

#include <utility>

RenderHandler s_RenderHandler;
KeyboardInputHandlerDelegate s_KeyboardInputHandler;
MouseMoveHandlerDelegate s_MouseMoveHandler;

GameWindow *GameWindow::Create(int width, int height,
                               RenderHandler renderHandler,
                               KeyboardInputHandlerDelegate keyboardInputHandler,
                               MouseMoveHandlerDelegate mouseMoveHandler)
{
    return new GameWindowOpenGL(width, height, std::move(renderHandler), std::move(keyboardInputHandler),
                                std::move(mouseMoveHandler));
}

GameWindow::GameWindow(RenderHandler renderHandler,
                       KeyboardInputHandlerDelegate keyboardInputHandler,
                       MouseMoveHandlerDelegate mouseMoveHandler)
{
    s_RenderHandler = std::move(renderHandler);
    s_KeyboardInputHandler = std::move(keyboardInputHandler);
    s_MouseMoveHandler = std::move(mouseMoveHandler);
}

void GameWindow::DrawInternal(int width, int height)
{
    if (s_RenderHandler)
    {
        s_RenderHandler(width, height);
    }

    TopMenuBar::Draw([this](){ SetCloseFlag(); });
    WindowManager::DrawAllWindows();
}

void GameWindow::HandleKeyboardInput(unsigned char key, bool pressed)
{
    if (s_KeyboardInputHandler)
    {
        s_KeyboardInputHandler(key, pressed);
    }
}

void GameWindow::HandleMouseMove(double x, double y)
{
    if (s_MouseMoveHandler)
    {
        s_MouseMoveHandler(x, y);
    }
}

const std::string &GameWindow::GetWindowTitle()
{
    static std::string title = "RenderEngine";
    return title;
}