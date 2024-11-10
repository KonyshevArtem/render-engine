#include "texture_binary_reader.h"
#include "utils/utils.h"

bool TextureBinaryReader::ReadTexture(const std::filesystem::path &path)
{
    static constexpr int headerSize = sizeof(TextureHeader);

    m_TextureBinaryData.clear();

    if (!Utils::ReadFileBytes(Utils::GetExecutableDirectory() / path, m_TextureBinaryData))
    {
        return false;
    }

    m_Header = *reinterpret_cast<TextureHeader*>(m_TextureBinaryData.data());

    auto *sizes = reinterpret_cast<uint32_t*>(&m_TextureBinaryData[0] + headerSize);
    m_MipSizes = std::span<uint32_t>(sizes, m_Header.Depth * m_Header.MipCount);

    uint32_t totalSize = GetMipsSize(m_Header.Depth - 1, m_Header.MipCount - 1);
    auto *pixels = reinterpret_cast<uint8_t*>(&m_TextureBinaryData[0] + headerSize + m_MipSizes.size() * sizeof(uint32_t));
    m_Pixels = std::span<uint8_t>(pixels, totalSize);

    return true;
}

std::span<uint8_t> TextureBinaryReader::GetPixels(unsigned int slice, unsigned int mipLevel) const
{
    if (slice >= m_Header.Depth || mipLevel >= m_Header.MipCount)
        return {};

    uint32_t mipSize = m_MipSizes[slice * m_Header.MipCount + mipLevel];
    uint32_t mipOffset = slice == 0 && mipLevel == 0 ? 0 : GetMipsSize(slice, mipLevel - 1);

    return {m_Pixels.data() + mipOffset, mipSize};
}

uint32_t TextureBinaryReader::GetMipsSize(unsigned int slices, unsigned int mipLevels) const
{
    uint32_t mipsSize = 0;
    for (int i = 0; i <= slices * m_Header.MipCount + mipLevels; ++i)
    {
        mipsSize += m_MipSizes[i];
    }
    return mipsSize;
}
