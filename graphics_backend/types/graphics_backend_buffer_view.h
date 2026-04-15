#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_H

#include <cstdint>

struct GraphicsBackendBufferView
{
public:
    uint32_t BindlessIndex = 0;

private:
    void* BufferView;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
