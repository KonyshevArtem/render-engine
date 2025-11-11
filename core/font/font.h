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

    inline const CommonBlock& GetCommonBlock() const
    {
        return m_Common;
    }

    inline std::shared_ptr<Texture> GetAtlas() const
    {
        return m_Atlas;
    }

    std::vector<Char> ShapeText(const std::span<const char> text);

private:
    CommonBlock m_Common;
    std::vector<uint8_t> m_FontBytes;
    std::shared_ptr<Texture> m_Atlas;
    std::shared_ptr<Trex::Atlas> m_TrexAtlas;
    std::string m_FontName;
};

#endif //RENDER_ENGINE_FONT_H
