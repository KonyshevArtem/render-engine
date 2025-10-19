#include "font_binary_reader.h"
#include "file_system/file_system.h"

bool FontBinaryReader::ReadFont(const std::filesystem::path &path)
{
    m_FontBinaryData.clear();

    if (!FileSystem::ReadFileBytes(FileSystem::GetResourcesPath() / path, m_FontBinaryData))
        return false;

    uint64_t offset = 0;

    Common = *reinterpret_cast<CommonBlock*>(m_FontBinaryData.data());
    offset += sizeof(CommonBlock);

    Char* chars = reinterpret_cast<Char*>(&m_FontBinaryData[offset]);
    Chars = std::span<Char>(chars, Common.CharsCount);
    offset += Chars.size_bytes();

    if (Common.KerningPairsCount)
    {
        KerningPair* kerningPairs = reinterpret_cast<KerningPair*>(&m_FontBinaryData[offset]);
        KerningPairs = std::span<KerningPair>(kerningPairs, Common.KerningPairsCount);
        offset += KerningPairs.size_bytes();
    }

    PagesHeader = *reinterpret_cast<TextureHeader*>(&m_FontBinaryData[offset]);
    offset += sizeof(TextureHeader);

    PageBytes.resize(Common.Pages);
    for (int i = 0; i < Common.Pages; ++i)
    {
        uint32_t pixelsSize = *reinterpret_cast<uint32_t*>(&m_FontBinaryData[offset]);
        offset += sizeof(uint32_t);

        uint8_t* pixels = reinterpret_cast<uint8_t*>(&m_FontBinaryData[offset]);
        PageBytes[i] = std::span<uint8_t>(pixels, pixelsSize);
        offset += pixelsSize;
    }

    return true;
}