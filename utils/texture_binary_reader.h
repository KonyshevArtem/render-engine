#ifndef OPENGL_STUDY_TEXTURE_BINARY_READER_H
#define OPENGL_STUDY_TEXTURE_BINARY_READER_H

#include "../core/texture/texture_header.h"

#include <filesystem>
#include <vector>
#include <span>

struct TextureHeader;

class TextureBinaryReader
{
public:
    TextureBinaryReader() = default;

    bool ReadTexture(const std::filesystem::path &path);
    std::span<uint8_t> GetPixels(unsigned int slice, unsigned int mipLevel) const;

    const TextureHeader &GetHeader() const
    {
        return m_Header;
    }

private:
    std::vector<uint8_t> m_TextureBinaryData;
    std::span<uint32_t> m_MipSizes;
    std::span<uint8_t> m_Pixels;
    TextureHeader m_Header{};

    uint32_t GetMipsSize(unsigned int slices, unsigned int mipLevels) const;
};


#endif //OPENGL_STUDY_TEXTURE_BINARY_READER_H
