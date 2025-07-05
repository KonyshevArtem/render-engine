#ifndef RENDER_ENGINE_PROFILER_WINDOW_H
#define RENDER_ENGINE_PROFILER_WINDOW_H

#ifdef ENABLE_IMGUI

#include "base_window.h"
#include "editor/profiler/profiler.h"
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
    struct DraggableContentRegion
    {
        DraggableContentRegion(const char* id, double rangeToWidth, float width, float height, ProfilerWindow* profilerWindow);
        ~DraggableContentRegion();

    private:
        ProfilerWindow* m_ProfilerWindow;
    };

    std::chrono::microseconds m_CurrentRange;
    std::chrono::microseconds m_Offset;
    bool m_IsEnabled;

    void HandleZoom();
    void HandleDrag(double rangeToWidth);
    void AddOffset(int offset);
    void DrawMarkers(const std::string& label, Profiler::MarkerContext context, const std::chrono::system_clock::time_point& rangeBegin, const std::chrono::system_clock::time_point& rangeEnd, double rangeToWidth);
};

#endif
#endif //RENDER_ENGINE_PROFILER_WINDOW_H