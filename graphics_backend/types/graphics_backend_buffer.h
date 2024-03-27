#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H

#include <cstdint>
#include "enums/buffer_bind_target.h"
#include "enums/buffer_usage_hint.h"

class GraphicsBackendBuffer
{
public:
    static GraphicsBackendBuffer NONE;

public:
    uint64_t Buffer;

    BufferBindTarget BindTarget;
    BufferUsageHint UsageHint;
    int Size;
    bool IsDataInitialized;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
