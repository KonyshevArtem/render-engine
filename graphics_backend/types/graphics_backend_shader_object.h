#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H

#include <cstdint>
#include "enums/shader_type.h"

class GraphicsBackendShaderObject
{
private:
    uint64_t ShaderObject;
    ShaderType ShaderType;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_SHADER_OBJECT_H
