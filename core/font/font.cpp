#include "font.h"
#include "Trex/Atlas.hpp"
#include "Trex/TextShaper.hpp"
#include "texture_2d/texture_2d.h"
#include "editor/profiler/profiler.h"

Font::Font(std::vector<uint8_t>& bytes, const std::string& fontName) :
    m_FontBytes(std::move(bytes)),
    m_FontName(fontName)
{
}

void Font::Prepare(uint16_t fontSize)
{
    if (m_Atlas.contains(fontSize))
        return;

    Profiler::Marker _("Font::Prepare");

    std::shared_ptr<Trex::Atlas> trexAtlas = std::make_shared<Trex::Atlas>(m_FontBytes, fontSize, Trex::Charset::Ascii());
    m_TrexAtlas[fontSize] = trexAtlas;

    const Trex::Atlas::Bitmap& bitmap = trexAtlas->GetBitmap();
    std::shared_ptr<Texture> atlas = Texture2D::Create(bitmap.Width(), bitmap.Height(), TextureInternalFormat::R8, true, false, m_FontName + "_Atlas_" + std::to_string(fontSize));
    atlas->UploadPixels(reinterpret_cast<const void*>(bitmap.Data().data()), bitmap.Data().size(), 0, 0);
    m_Atlas[fontSize] = atlas;

    CommonBlock commonBlock;
    commonBlock.LineHeight = trexAtlas->GetFont()->GetMetrics().height;
    commonBlock.ScaleW = bitmap.Width();
    commonBlock.ScaleH = bitmap.Height();
    m_Common[fontSize] = commonBlock;
}

const CommonBlock& Font::GetCommonBlock(uint16_t fontSize) const
{
    static CommonBlock empty{};

    auto it = m_Common.find(fontSize);
    return it != m_Common.end() ? it->second : empty;
}

const std::shared_ptr<Texture> Font::GetAtlas(uint16_t fontSize) const
{
    auto it = m_Atlas.find(fontSize);
    return it != m_Atlas.end() ? it->second : nullptr;
}

std::vector<Char> Font::ShapeText(const std::span<const char> text, uint16_t fontSize)
{
    Profiler::Marker _("Font::ShapeText");

    auto it = m_TrexAtlas.find(fontSize);
    if (it == m_TrexAtlas.end())
        return std::vector<Char>();

    std::shared_ptr<Trex::Atlas> trexAtlas = it->second;

    Trex::TextShaper shaper(*trexAtlas);
    Trex::ShapedGlyphs glyphs = shaper.ShapeUtf8(text);

    std::vector<Char> chars;
    chars.reserve(glyphs.size());

    for (const Trex::ShapedGlyph& glyph : glyphs)
    {
        Char ch{};
        ch.XOffset = glyph.xOffset + glyph.info.bearingX;
        ch.YOffset = glyph.yOffset - glyph.info.bearingY + trexAtlas->GetFont()->GetMetrics().ascender;
        ch.XAdvance = glyph.xAdvance;
        ch.Width = glyph.info.width;
        ch.Height = glyph.info.height;
        ch.X = glyph.info.x;
        ch.Y = glyph.info.y;
        chars.push_back(ch);
    }

    return chars;
}