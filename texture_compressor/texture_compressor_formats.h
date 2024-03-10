#ifndef RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H
#define RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H

#include <string>
#include <vector>
#include "enums/texture_type.h"
#include "enums/texture_pixel_format.h"
#include "enums/texture_internal_format.h"

struct TextureTypeInfo
{
public:
    std::string Name;
    TextureType Type;
    int Count;
};

namespace TextureCompressorFormats
{
    std::string GetTextureFormatName(TextureInternalFormat format);
    const TextureTypeInfo &GetTextureTypeInfo(TextureType textureType);
    TexturePixelFormat GetPixelFormatByColorType(int colorType);

    const std::vector<TextureTypeInfo> &GetTextureTypesInfo();
    const std::vector<std::pair<int, std::string>> &GetInputFormats();
    const std::vector<std::pair<TextureInternalFormat, std::string>> &GetTextureFormats();
}

#endif //RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H
