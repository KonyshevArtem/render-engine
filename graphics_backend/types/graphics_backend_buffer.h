#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H

#include <cstdint>
#include "enums/buffer_usage_hint.h"

class GraphicsBackendBuffer
{
public:
    uint64_t Buffer;
    int Size;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
