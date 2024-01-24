#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_VAO_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_VAO_H

#include "graphics_backend.h"

class GraphicsBackendVAO
{
public:
    static GraphicsBackendVAO NONE;

private:
    GRAPHICS_BACKEND_TYPE_UINT VAO;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_VAO_H
