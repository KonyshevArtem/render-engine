#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H

#include "graphics_backend.h"

class GraphicsBackendVAO
{
public:
    static GraphicsBackendVAO NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT VAO;

    friend class GraphicsBackend;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_VAO_H
