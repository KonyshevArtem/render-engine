#ifndef RENDER_ENGINE_UNIFORM_INFO_H
#define RENDER_ENGINE_UNIFORM_INFO_H

#include "types/graphics_backend_resource_bindings.h"
#include "enums/texture_data_type.h"

struct GraphicsBackendTextureInfo
{
    TextureDataType Type = TextureDataType::SAMPLER_2D;
    GraphicsBackendResourceBindings TextureBindings{};
    bool HasSampler = false;
};

#endif //RENDER_ENGINE_UNIFORM_INFO_H
