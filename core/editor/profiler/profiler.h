#ifndef PROFILER_MARKERS_H
#define PROFILER_MARKERS_H

#include "types/graphics_backend_profiler_marker.h"

#include <chrono>
#include <vector>

class Profiler
{
public:
    enum class MarkerType
    {
        MARKER,
        SEPARATOR
    };

    enum class MarkerContext
    {
        MAIN_THREAD,
        GPU_RENDER,

        MAX
    };

    struct MarkerInfo
    {
        const MarkerType Type = MarkerType::MARKER;
        std::chrono::system_clock::time_point Begin;
        std::chrono::system_clock::time_point End;
        const char* Name = nullptr;
        const int Depth = 0;

        explicit MarkerInfo(MarkerType type, const char* name = nullptr, int depth = 0);
    };

    struct GPUMarkerInfo
    {
        GraphicsBackendProfilerMarker ProfilerMarker;
        const char* Name = nullptr;
        int Depth = 0;
        int FrameNumber = 0;

        GPUMarkerInfo(const char* name, int depth, int frameNumber);
        GPUMarkerInfo& operator=(const GPUMarkerInfo& info);
    };

    struct Marker
    {
        explicit Marker(const char* name);
        ~Marker();

    private:
        MarkerContext m_Context;
        MarkerInfo m_Info;
    };

    struct GPUMarker
    {
        explicit GPUMarker(const char* name);
        ~GPUMarker();

    private:
        MarkerContext m_Context;
        GPUMarkerInfo m_Info;
    };

    struct FrameInfo
    {
        std::vector<MarkerInfo> Markers;
        int FrameNumber;
        int MaxDepth;
    };

    static void SetEnabled(bool enabled);
    static void BeginNewFrame();
    static std::vector<FrameInfo>& GetContextFrames(MarkerContext context);

private:
    static void AddMarkerInfo(MarkerContext context, const MarkerInfo& markerInfo, int frame = -1);
};

#endif //PROFILER_MARKERS_H
