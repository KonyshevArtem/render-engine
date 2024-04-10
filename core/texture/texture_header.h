#ifndef RENDER_ENGINE_TEXTURE_HEADER_H
#define RENDER_ENGINE_TEXTURE_HEADER_H

#include "enums/texture_internal_format.h"

struct TextureHeader
{
public:
    int Width;
    int Height;
    int Depth;
    TextureInternalFormat TextureFormat;
    unsigned int MipCount;
};

#endif