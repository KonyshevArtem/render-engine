#include "font.h"

Font::Font(const CommonBlock& commonBlock, const std::span<Char>& chars, const std::span<KerningPair>& kerningPairs, const std::shared_ptr<Texture>& atlas) :
    m_Common(commonBlock),
    m_Atlas(atlas)
{
    for (const Char& ch : chars)
        m_Chars[ch.Id] = ch;

    for (const KerningPair& pair : kerningPairs)
    {
        uint64_t key = (static_cast<uint64_t>(pair.First) << 32) | pair.Second;
        m_KerningPairs[key] = pair.Amount;
    }
}

const Char& Font::GetChar(uint32_t ch) const
{
    static Char emptyChar{};

    auto it = m_Chars.find(ch);
    return it != m_Chars.end() ? it->second : emptyChar;
}

int16_t Font::GetKerning(uint32_t firstChar, uint32_t secondChar) const
{
    uint64_t key = (static_cast<uint64_t>(firstChar) << 32) | secondChar;
    auto it = m_KerningPairs.find(key);
    return it != m_KerningPairs.end() ? it->second : 0;
}