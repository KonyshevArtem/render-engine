#ifndef RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H
#define RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H

#include <string>
#include <vector>

#include "cuttlefish/Texture.h"
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"

struct TextureTypeInfo
{
    std::string Name;
    TextureType Type;
    cuttlefish::Texture::Dimension CuttlefishDimensions;
    int Count;
};

struct TextureFormatInfo
{
    TextureInternalFormat Format;
    std::string Name;
    cuttlefish::Texture::Format CuttlefishFormat;
    cuttlefish::Texture::Type CuttlefishType;
};

namespace TextureCompressorFormats
{
    const TextureFormatInfo& GetTextureFormatInfo(const std::string& format);
    const TextureTypeInfo& GetTextureTypeInfo(const std::string& textureType);
}

#endif //RENDER_ENGINE_TEXTURE_COMPRESSOR_FORMATS_H
