#ifndef OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H
#define OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H

#include <string>
#include <vector>

struct TextureTypeInfo
{
public:
    std::string Name;
    int TypeGL;
    int Count;
};

namespace TextureCompressorFormats
{
    std::string GetCompressedFormatName(int format);
    const TextureTypeInfo &GetTextureTypeInfo(int textureType);
    int GetFormatByColorType(int colorType);

    const std::vector<TextureTypeInfo> &GetTextureTypesInfo();
    const std::vector<std::pair<int, std::string>> &GetInputFormats();
    const std::vector<std::pair<int, std::string>> &GetCompressedFormats();
}

#endif //OPENGL_STUDY_TEXTURE_COMPRESSOR_FORMATS_H
