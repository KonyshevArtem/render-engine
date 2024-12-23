#ifndef RENDER_ENGINE_GAME_WINDOW_H
#define RENDER_ENGINE_GAME_WINDOW_H

#include <functional>

typedef std::function<void(int, int)> RenderHandler;

class GameWindow
{
public:
    explicit GameWindow(RenderHandler renderHandler);
    ~GameWindow() = default;

    void TickMainLoop(int width, int height);

    bool ShouldCloseWindow() const;
    bool CaptureKeyboard() const;
    bool CaptureMouse() const;

private:
    bool m_CloseFlag = false;

    RenderHandler m_RenderHandler;
};

#endif