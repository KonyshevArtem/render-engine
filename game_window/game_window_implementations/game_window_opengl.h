#ifndef RENDER_ENGINE_GAME_WINDOW_OPENGL_H
#define RENDER_ENGINE_GAME_WINDOW_OPENGL_H

#include "game_window.h"

struct GLFWwindow;

class GameWindowOpenGL : public GameWindow
{
public:
    GameWindowOpenGL(int width, int height,
                     RenderHandler renderHandler,
                     KeyboardInputHandlerDelegate keyboardInputHandler,
                     MouseMoveHandlerDelegate mouseMoveHandler);
    ~GameWindowOpenGL() override;

    void BeginMainLoop() override;

protected:
    void SetCloseFlag() override;

private:
    GLFWwindow* m_WindowPtr;

    static void KeyboardFunction(GLFWwindow* window, int keycode, int scancode, int action, int mods);
    static void MouseMoveFunction(GLFWwindow *window, double x, double y);
};


#endif //RENDER_ENGINE_GAME_WINDOW_OPENGL_H
