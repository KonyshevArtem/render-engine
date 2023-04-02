#ifndef TEXTURE_COMPRESSOR_BACKEND_H
#define TEXTURE_COMPRESSOR_BACKEND_H

#include <string>
#include <vector>

struct TextureInfo
{
    unsigned int Width;
    unsigned int Height;
    std::string Format;
    int Size;
};

namespace TextureCompressorBackend
{
    void InitInteractiveMode();
    void RenderInteractiveMode();
    TextureInfo LoadTexture(const std::string &path);
    void CompressTexture(const std::string& path, int colorType, int compressedFormat);

    const std::vector<std::pair<int, std::string>> &GetInputFormats();
    const std::vector<std::pair<int, std::string>> &GetCompressedFormats();
}

#endif