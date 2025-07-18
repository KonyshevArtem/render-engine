#ifndef TEXTURE_COMPRESSOR_BACKEND_H
#define TEXTURE_COMPRESSOR_BACKEND_H

#include <string>
#include <filesystem>

namespace TextureCompressorBackend
{
    void CompressTextures(const std::filesystem::path& inputPaths, const std::filesystem::path& outputPath, const std::string& platform);
}

#endif