#ifndef OPENGL_STUDY_WINDOW
#define OPENGL_STUDY_WINDOW

#include <functional>

typedef std::function<void(int, int)> RenderHandler;
typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(double, double)> MouseMoveHandlerDelegate;

struct GLFWwindow;

class GameWindow
{
public:
    GameWindow(int width,
               int height,
               RenderHandler renderHandler,
               KeyboardInputHandlerDelegate keyboardInputHandler,
               MouseMoveHandlerDelegate mouseMoveHandler);

    virtual ~GameWindow();

    void BeginMainLoop();

private:
    GLFWwindow* m_WindowPtr;

    void SetCloseFlag();
};

#endif