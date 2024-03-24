#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H

#include "graphics_backend.h"

class GraphicsBackendVAO
{
public:
    static GraphicsBackendVAO NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT VAO;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H
