#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H

#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"
#include <cstdint>

class GraphicsBackendTexture
{
private:
    uint64_t Texture;
    TextureType Type;
    TextureInternalFormat Format;
    bool IsLinear;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_H
