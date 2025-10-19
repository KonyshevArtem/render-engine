#ifndef RENDER_ENGINE_FONT_BINARY_READER_H
#define RENDER_ENGINE_FONT_BINARY_READER_H

#include "font.h"
#include "texture/texture_header.h"

#include <span>
#include <vector>
#include <filesystem>

class FontBinaryReader
{
public:
    FontBinaryReader() = default;

    bool ReadFont(const std::filesystem::path& path);

    CommonBlock Common;
    std::span<Char> Chars;
    std::span<KerningPair> KerningPairs;

    TextureHeader PagesHeader;
    std::vector<std::span<uint8_t>> PageBytes;

private:
    std::vector<uint8_t> m_FontBinaryData;
};

#endif //RENDER_ENGINE_FONT_BINARY_READER_H
