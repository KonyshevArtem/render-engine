#ifndef OPENGL_STUDY_TEXTURE_HEADER_H
#define OPENGL_STUDY_TEXTURE_HEADER_H

#include "enums/texture_internal_format.h"
#include "enums/texture_pixel_format.h"

struct TextureHeader
{
public:
    int Width;
    int Height;
    int Depth;
    TextureInternalFormat TextureFormat;
    TexturePixelFormat PixelFormat;
    int IsCompressed;
    unsigned int MipCount;
};

#endif