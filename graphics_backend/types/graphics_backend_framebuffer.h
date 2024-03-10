#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_FRAMEBUFFER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_FRAMEBUFFER_H

#include "graphics_backend.h"

class GraphicsBackendFramebuffer
{
public:
    static GraphicsBackendFramebuffer NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Framebuffer;

    friend class GraphicsBackend;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_FRAMEBUFFER_H
