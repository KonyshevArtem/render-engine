#ifndef OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H
#define OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H

#include <vector>
#include <unordered_map>

namespace TextureCompressorFormats
{
    std::string GetCompressedFormatName(int format);
    int GetFormatByColorType(int colorType);

    const std::vector<std::pair<int, std::string>> &GetInputFormats();
    const std::vector<std::pair<int, std::string>> &GetCompressedFormats();
}

#endif //OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H
