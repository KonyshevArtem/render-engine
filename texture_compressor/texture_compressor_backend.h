#ifndef TEXTURE_COMPRESSOR_BACKEND_H
#define TEXTURE_COMPRESSOR_BACKEND_H

#include <string>
#include <vector>

namespace TextureCompressorBackend
{
    void CompressTexture(const std::string& path, int colorType, int compressedFormat);

    const std::vector<std::pair<int, std::string>> &GetInputFormats();
    const std::vector<std::pair<int, std::string>> &GetCompressedFormats();
}

#endif