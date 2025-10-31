#ifndef RENDER_ENGINE_FONT_H
#define RENDER_ENGINE_FONT_H

#include "font_data.h"
#include "resources/resource.h"

#include <unordered_map>
#include <memory>
#include <span>

class Texture;

class Font : public Resource
{
public:
    Font(const CommonBlock& commonBlock, const std::span<Char>& chars, const std::span<KerningPair>& kerningPairs, const std::shared_ptr<Texture>& atlas);
    ~Font() = default;

    inline const CommonBlock& GetCommonBlock() const
    {
        return m_Common;
    }

    inline std::shared_ptr<Texture> GetAtlas() const
    {
        return m_Atlas;
    }

    const Char& GetChar(uint32_t ch) const;
    int16_t GetKerning(uint32_t firstChar, uint32_t secondChar) const;

private:
    CommonBlock m_Common;
    std::unordered_map<uint32_t, Char> m_Chars;
    std::unordered_map<uint64_t, int16_t> m_KerningPairs;
    std::shared_ptr<Texture> m_Atlas;
};

#endif //RENDER_ENGINE_FONT_H
