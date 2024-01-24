#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_TEXTURE_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_TEXTURE_H

#include "graphics_backend.h"

class GraphicsBackendTexture
{
public:
    static GraphicsBackendTexture NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Texture;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_TEXTURE_H
