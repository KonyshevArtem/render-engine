#ifndef RENDER_ENGINE_UNIFORM_INFO_H
#define RENDER_ENGINE_UNIFORM_INFO_H

#include "types/graphics_backend_resource_bindings.h"
#include "enums/uniform_data_type.h"

struct GraphicsBackendUniformInfo
{
    UniformDataType Type = UniformDataType::FLOAT;
    int Location = 0;
    GraphicsBackendResourceBindings TextureBindings{};
    bool IsTexture = false;
    bool HasSampler = false;
};

#endif //RENDER_ENGINE_UNIFORM_INFO_H
