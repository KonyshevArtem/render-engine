#ifndef RENDER_ENGINE_TEXTURE_HEADER_H
#define RENDER_ENGINE_TEXTURE_HEADER_H

#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"

struct TextureHeader
{
    TextureType Type;
    uint16_t Width;
    uint16_t Height;
    uint16_t Depth;
    TextureInternalFormat TextureFormat;
    uint8_t MipCount;
    uint8_t IsLinear : 1;
};

#endif