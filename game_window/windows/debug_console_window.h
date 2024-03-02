#ifndef OPENGL_STUDY_DEBUG_CONSOLE_WINDOW_H
#define OPENGL_STUDY_DEBUG_CONSOLE_WINDOW_H

#include "base_window.h"

#include <vector>

class DebugConsoleWindow : public BaseWindow
{
public:
    DebugConsoleWindow();
    ~DebugConsoleWindow() override;

protected:
    void DrawInternal() override;

private:
    static class _init
    {
    public:
        _init();
    } init;

    void OnDebugLog(std::string string);

    int m_Selected = -1;
    bool m_Paused = false;
    std::vector<std::string> m_Logs;
};

#endif //OPENGL_STUDY_DEBUG_CONSOLE_WINDOW_H
