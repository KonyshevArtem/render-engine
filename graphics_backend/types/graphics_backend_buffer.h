#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H

#include <cstdint>

class GraphicsBackendBuffer
{
public:
    uint64_t Buffer;
    uint32_t Size;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
