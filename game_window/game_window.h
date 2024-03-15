#ifndef RENDER_ENGINE_WINDOW
#define RENDER_ENGINE_WINDOW

#include <functional>

typedef std::function<void(int, int)> RenderHandler;
typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(double, double)> MouseMoveHandlerDelegate;

class GameWindow
{
public:
    static GameWindow* Create(int width, int height,
                              RenderHandler renderHandler,
                              KeyboardInputHandlerDelegate keyboardInputHandler,
                              MouseMoveHandlerDelegate mouseMoveHandler);

    virtual ~GameWindow() = default;

    virtual void BeginMainLoop() = 0;

protected:
    GameWindow(RenderHandler renderHandler,
               KeyboardInputHandlerDelegate keyboardInputHandler,
               MouseMoveHandlerDelegate mouseMoveHandler);

    void DrawInternal(int width, int height);

protected:
    virtual void SetCloseFlag() = 0;

    static void HandleKeyboardInput(unsigned char key, bool pressed);
    static void HandleMouseMove(double x, double y);
    static const std::string &GetWindowTitle();
};

#endif