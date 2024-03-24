#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H

#include "graphics_backend.h"

class GraphicsBackendBuffer
{
public:
    static GraphicsBackendBuffer NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Buffer;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_H
