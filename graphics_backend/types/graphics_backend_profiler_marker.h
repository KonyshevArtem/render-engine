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

    MarkerInfo Info[static_cast<int>(GPUQueue::MAX)];
    uint64_t Frame;

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

using ProfilerMarkerResolveResults = ProfilerMarkerResolveResult[static_cast<int>(GPUQueue::MAX)];

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H
