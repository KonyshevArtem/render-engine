#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_STENCIL_STATE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_STENCIL_STATE_H

#include <cstdint>

struct GraphicsBackendDepthStencilState
{
private:
    uint64_t m_State;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_STENCIL_STATE_H
