#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_VIEW_H

#include <cstdint>

class GraphicsBackendBufferView
{
public:
    void* BufferView;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
