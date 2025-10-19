#ifndef RENDER_ENGINE_FONT_H
#define RENDER_ENGINE_FONT_H

#include "font_data.h"
#include "resources/resource.h"

#include <unordered_map>
#include <memory>
#include <span>

class Texture2DArray;

class Font : public Resource
{
public:
    Font(const CommonBlock& commonBlock, const std::span<Char>& chars, const std::span<KerningPair>& kerningPairs, const std::shared_ptr<Texture2DArray>& atlas);
    ~Font() = default;

    inline std::shared_ptr<Texture2DArray> GetAtlas() const
    {
        return m_Atlas;
    }

private:
    std::unordered_map<uint32_t, Char> m_Chars;
    std::unordered_map<uint64_t, int16_t> m_KerningPairs;
    std::shared_ptr<Texture2DArray> m_Atlas;
};

#endif //RENDER_ENGINE_FONT_H
