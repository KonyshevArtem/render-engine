#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_FENCE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_FENCE_H

#include "enums/fence_type.h"
#include <cstdint>

struct GraphicsBackendFence
{
private:
    uint64_t Fence;
    FenceType Type;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_FENCE_H
