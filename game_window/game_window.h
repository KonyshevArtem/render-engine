#ifndef OPENGL_STUDY_WINDOW
#define OPENGL_STUDY_WINDOW

#include <functional>

typedef std::function<void(int, int)> ResizeHandler;
typedef std::function<void()> RenderHandler;
typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(int, int)> MouseMoveHandlerDelegate;

class GameWindow
{
public:
    GameWindow(int width,
               int height,
               ResizeHandler resizeHandler,
               RenderHandler renderHandler,
               KeyboardInputHandlerDelegate keyboardInputHandler,
               MouseMoveHandlerDelegate mouseMoveHandler);

    virtual ~GameWindow();

    void BeginMainLoop();
};

#endif