#ifndef RENDER_ENGINE_FONT_H
#define RENDER_ENGINE_FONT_H

#include "font_data.h"
#include "resources/resource.h"

#include <unordered_map>
#include <memory>
#include <span>
#include <string>

class Texture;

namespace Trex
{
    class Atlas;
}

class Font : public Resource
{
public:
    Font(std::vector<uint8_t>& bytes, const std::string& fontName);
    ~Font() = default;

    void Prepare(uint16_t fontSize);

    const CommonBlock& GetCommonBlock(uint16_t fontSize) const;
    const std::shared_ptr<Texture> GetAtlas(uint16_t fontSize) const;

    std::vector<Char> ShapeText(const std::span<const char> text, uint16_t fontSize);

private:
    std::unordered_map<uint16_t, CommonBlock> m_Common;
    std::unordered_map<uint16_t, std::shared_ptr<Texture>> m_Atlas;
    std::unordered_map<uint16_t, std::shared_ptr<Trex::Atlas>> m_TrexAtlas;

    std::vector<uint8_t> m_FontBytes;
    std::string m_FontName;
};

#endif //RENDER_ENGINE_FONT_H
