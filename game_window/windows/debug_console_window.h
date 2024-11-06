#ifndef RENDER_ENGINE_DEBUG_CONSOLE_WINDOW_H
#define RENDER_ENGINE_DEBUG_CONSOLE_WINDOW_H

#ifdef ENABLE_IMGUI

#include "base_window.h"
#include "window_manager.h"

#include <vector>

class DebugConsoleWindow : public BaseWindow
{
public:
    DebugConsoleWindow();
    ~DebugConsoleWindow() override;

protected:
    void DrawInternal() override;

private:
    DECLARE_STATIC_CONSTRUCTOR;

    void OnDebugLog(std::string string);

    int m_Selected = -1;
    bool m_Paused = false;
    bool m_NewLogAdded = false;
    std::vector<std::string> m_Logs;
};

#endif

#endif //RENDER_ENGINE_DEBUG_CONSOLE_WINDOW_H
