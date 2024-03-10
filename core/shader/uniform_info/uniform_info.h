#ifndef RENDER_ENGINE_UNIFORM_INFO_H
#define RENDER_ENGINE_UNIFORM_INFO_H

#include "enums/texture_unit.h"
#include "enums/uniform_data_type.h"
#include "types/graphics_backend_uniform_location.h"

struct UniformInfo
{
    UniformDataType Type = UniformDataType::FLOAT;
    GraphicsBackendUniformLocation Location{};
    TextureUnit TextureUnit = TextureUnit::TEXTURE0;
    bool IsTexture = false;
};

#endif //RENDER_ENGINE_UNIFORM_INFO_H
