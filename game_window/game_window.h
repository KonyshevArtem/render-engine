#ifndef RENDER_ENGINE_GAME_WINDOW_H
#define RENDER_ENGINE_GAME_WINDOW_H

class GameWindow
{
public:
    GameWindow() = default;
    ~GameWindow();

    void Render() const;

    bool ShouldCloseWindow() const;
    bool CaptureKeyboard() const;
    bool CaptureMouse() const;

private:
    bool m_CloseFlag = false;
};

#endif