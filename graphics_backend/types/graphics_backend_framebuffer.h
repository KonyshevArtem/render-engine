#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_FRAMEBUFFER_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_FRAMEBUFFER_H

#include "graphics_backend.h"

class GraphicsBackendFramebuffer
{
public:
    static GraphicsBackendFramebuffer NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT Framebuffer;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_FRAMEBUFFER_H
