#ifndef RENDER_ENGINE_PROFILER_WINDOW_H
#define RENDER_ENGINE_PROFILER_WINDOW_H

#ifdef ENABLE_IMGUI

#include "base_window.h"
#include <chrono>

class ProfilerWindow : public BaseWindow
{
public:
    ProfilerWindow();
    ~ProfilerWindow() override;

protected:
    void DrawTopBar() override;
    void DrawInternal() override;

private:
    std::chrono::microseconds m_CurrentRange;
    std::chrono::microseconds m_Offset;
    bool m_IsEnabled;

    void HandleZoom();
    void HandleDrag(double rangeToWidth);
};

#endif
#endif //RENDER_ENGINE_PROFILER_WINDOW_H