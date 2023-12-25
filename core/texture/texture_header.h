#ifndef OPENGL_STUDY_TEXTURE_HEADER_H
#define OPENGL_STUDY_TEXTURE_HEADER_H

struct TextureHeader
{
    public:
        unsigned int Width;
        unsigned int Height;
        unsigned int Depth;
        int          InternalFormat;
        int          Format;
        int          IsCompressed;
        unsigned int MipCount;
};

#endif