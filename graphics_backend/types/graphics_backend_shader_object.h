#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_SHADER_OBJECT_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_SHADER_OBJECT_H

#include "graphics_backend.h"

class GraphicsBackendShaderObject
{
private:
    GRAPHICS_BACKEND_TYPE_UINT ShaderObject;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_SHADER_OBJECT_H
