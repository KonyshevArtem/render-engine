#ifndef OPENGL_STUDY_WINDOW
#define OPENGL_STUDY_WINDOW

#include <functional>

typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(int, int)> MouseMoveHandlerDelegate;

class GameWindow {
public:
    GameWindow(int width,
               int height,
               std::function<void(int, int)> _resize,
               std::function<void()> _render,
               KeyboardInputHandlerDelegate _keyboardInputHandler,
               MouseMoveHandlerDelegate _mouseMoveHandler);

    virtual ~GameWindow() = default;

protected:
    static void resizeGL(int _width, int _height);

    static void paintGL();

    static void keyPressEvent(unsigned char key, int x, int y);

    static void keyReleaseEvent(unsigned char key, int x, int y);

    static void mouseMoveEvent(int x, int y);

private:
    std::function<void(int, int)> m_Resize;
    std::function<void()> m_Render;
    KeyboardInputHandlerDelegate m_KeyboardInputHandler;
    MouseMoveHandlerDelegate m_MouseMoveHandler;
};

#endif