#ifndef RENDER_ENGINE_GAME_WINDOW_H
#define RENDER_ENGINE_GAME_WINDOW_H

#include <functional>

typedef std::function<void(int, int)> RenderHandler;
typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(double, double)> MouseMoveHandlerDelegate;

class GameWindow
{
public:
    GameWindow(RenderHandler renderHandler,
               KeyboardInputHandlerDelegate keyboardInputHandler,
               MouseMoveHandlerDelegate mouseMoveHandler);
    ~GameWindow();

    void TickMainLoop(int width, int height);

    void ProcessMouseMove(float x, float y);
    void ProcessKeyPress(char key, bool pressed);

    bool ShouldCloseWindow() const;

private:
    bool m_CloseFlag = false;

    RenderHandler m_RenderHandler;
    KeyboardInputHandlerDelegate m_KeyboardInputHandler;
    MouseMoveHandlerDelegate  m_MouseMoveHandler;
};

#endif