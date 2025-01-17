#ifndef TEXTURE_COMPRESSOR_BACKEND_H
#define TEXTURE_COMPRESSOR_BACKEND_H

#include <string>
#include <vector>
#include <filesystem>

namespace TextureCompressorBackend
{
    void CompressTexture(const std::vector<std::string>& paths, const std::string& textureType, const std::string& textureFormat,
                        bool isLinear, bool generateMips, const std::filesystem::path& outputPath);
}

#endif