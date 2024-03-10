#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H

#include "graphics_backend.h"

class GraphicsBackendShaderObject
{
private:
    GRAPHICS_BACKEND_TYPE_UINT ShaderObject;

    friend class GraphicsBackend;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H
