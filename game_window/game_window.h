#ifndef RENDER_ENGINE_GAME_WINDOW_H
#define RENDER_ENGINE_GAME_WINDOW_H

#include <functional>

typedef std::function<void(int, int)> RenderHandler;
typedef std::function<void(unsigned char, bool)> KeyboardInputHandlerDelegate;
typedef std::function<void(double, double)> MouseMoveHandlerDelegate;

class GameWindow
{
public:
    static GameWindow* Create(void* viewPtr,
                              RenderHandler renderHandler,
                              KeyboardInputHandlerDelegate keyboardInputHandler,
                              MouseMoveHandlerDelegate mouseMoveHandler);

    virtual ~GameWindow() = default;

    virtual void TickMainLoop(int width, int height) = 0;

    void ProcessMouseMove(float x, float y);
    void ProcessKeyPress(char key, bool pressed);

    bool ShouldCloseWindow() const;

protected:
    void DrawInternal(int width, int height);

private:
    bool m_CloseFlag = false;
};

#endif