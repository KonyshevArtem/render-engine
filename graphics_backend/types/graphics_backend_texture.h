#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H

#include "graphics_backend.h"

class GraphicsBackendTexture
{
public:
    static GraphicsBackendTexture NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Texture;

    friend class GraphicsBackend;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H
