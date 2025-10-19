#include "font.h"

Font::Font(const CommonBlock& commonBlock, const std::span<Char>& chars, const std::span<KerningPair>& kerningPairs, const std::shared_ptr<Texture2DArray>& atlas) :
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