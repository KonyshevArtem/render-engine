#ifndef RENDER_ENGINE_UNIFORM_INFO_H
#define RENDER_ENGINE_UNIFORM_INFO_H

#include "enums/texture_unit.h"
#include "enums/uniform_data_type.h"

struct GraphicsBackendUniformInfo
{
    UniformDataType Type = UniformDataType::FLOAT;
    int Location = 0;
    TextureUnit TextureUnit = TextureUnit::TEXTURE0;
    bool IsTexture = false;
};

#endif //RENDER_ENGINE_UNIFORM_INFO_H
