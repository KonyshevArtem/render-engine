#ifndef RENDER_ENGINE_TEXTURE_TYPE_H
#define RENDER_ENGINE_TEXTURE_TYPE_H

#include "graphics_backend.h"

enum class TextureType : GRAPHICS_BACKEND_TYPE_ENUM
{
    TEXTURE_1D          = GRAPHICS_BACKEND_TEXTURE_1D,
    TEXTURE_1D_ARRAY    = GRAPHICS_BACKEND_TEXTURE_1D_ARRAY,

    TEXTURE_2D                      = GRAPHICS_BACKEND_TEXTURE_2D,
    TEXTURE_2D_MULTISAMPLE          = GRAPHICS_BACKEND_TEXTURE_2D_MULTISAMPLE,
    TEXTURE_2D_ARRAY                = GRAPHICS_BACKEND_TEXTURE_2D_ARRAY,
    TEXTURE_2D_MULTISAMPLE_ARRAY    = GRAPHICS_BACKEND_TEXTURE_2D_MULTISAMPLE_ARRAY,

    TEXTURE_3D = GRAPHICS_BACKEND_TEXTURE_3D,

    TEXTURE_CUBEMAP         = GRAPHICS_BACKEND_TEXTURE_CUBEMAP,
    TEXTURE_CUBEMAP_ARRAY   = GRAPHICS_BACKEND_TEXTURE_CUBEMAP_ARRAY,

    TEXTURE_RECTANGLE   = GRAPHICS_BACKEND_TEXTURE_RECTANGLE,
    TEXTURE_BUFFER      = GRAPHICS_BACKEND_TEXTURE_BUFFER,
};

#endif //RENDER_ENGINE_TEXTURE_TYPE_H
