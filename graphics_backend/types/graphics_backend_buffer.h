#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_BUFFER_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_BUFFER_H

#include "graphics_backend.h"

class GraphicsBackendBuffer
{
public:
    static GraphicsBackendBuffer NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Buffer;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_BUFFER_H
