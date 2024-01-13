#ifndef TEXTURE_COMPRESSOR_BACKEND_H
#define TEXTURE_COMPRESSOR_BACKEND_H

#include <string>
#include <vector>
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"

namespace TextureCompressorBackend
{
    void CompressTexture(const std::vector<std::string> &paths, TextureType textureType, int colorType, TextureInternalFormat textureFormat, bool generateMips);
}

#endif