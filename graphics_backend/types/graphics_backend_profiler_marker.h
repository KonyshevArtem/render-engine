#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H

#include <cstdint>

struct GraphicsBackendProfilerMarker
{
private:
    uint64_t StartMarker;
    uint64_t EndMarker;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROFILER_MARKER_H
