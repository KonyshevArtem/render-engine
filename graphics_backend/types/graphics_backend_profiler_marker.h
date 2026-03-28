#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H

#include "enums/gpu_queue.h"
#include <cstdint>

struct GraphicsBackendProfilerMarker
{
private:
    struct MarkerInfo
    {
        uint64_t StartMarker;
        uint64_t EndMarker;
    };

    MarkerInfo Info;
    uint64_t Frame;
    GPUQueue Queue;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
};

struct ProfilerMarkerResolveResult
{
    bool IsActive;
    uint64_t StartTimestamp;
    uint64_t EndTimestamp;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H
