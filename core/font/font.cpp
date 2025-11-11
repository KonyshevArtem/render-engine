#include "font.h"
#include "Trex/Atlas.hpp"
#include "Trex/TextShaper.hpp"
#include "texture_2d/texture_2d.h"

Font::Font(std::vector<uint8_t>& bytes, const std::string& fontName) :
    m_FontBytes(std::move(bytes)),
    m_FontName(fontName)
{
    m_TrexAtlas = std::make_shared<Trex::Atlas>(m_FontBytes, 32, Trex::Charset::Ascii());

    const Trex::Atlas::Bitmap& bitmap = m_TrexAtlas->GetBitmap();
    m_Atlas = Texture2D::Create(bitmap.Width(), bitmap.Height(), TextureInternalFormat::R8, true, false, m_FontName + "_Atlas");
    m_Atlas->UploadPixels(reinterpret_cast<const void*>(bitmap.Data().data()), bitmap.Data().size(), 0, 0);

    m_Common.LineHeight = m_TrexAtlas->GetFont()->GetMetrics().height;
    m_Common.ScaleW = bitmap.Width();
    m_Common.ScaleH = bitmap.Height();
}

std::vector<Char> Font::ShapeText(const std::span<const char> text)
{
    Trex::TextShaper shaper(*m_TrexAtlas);
    Trex::ShapedGlyphs glyphs = shaper.ShapeUtf8(text);

    std::vector<Char> chars;
    chars.reserve(glyphs.size());

    for (const Trex::ShapedGlyph& glyph : glyphs)
    {
        Char ch{};
        ch.XOffset = glyph.xOffset + glyph.info.bearingX;
        ch.YOffset = glyph.yOffset - glyph.info.bearingY + m_TrexAtlas->GetFont()->GetMetrics().ascender;
        ch.XAdvance = glyph.xAdvance;
        ch.Width = glyph.info.width;
        ch.Height = glyph.info.height;
        ch.X = glyph.info.x;
        ch.Y = glyph.info.y;
        chars.push_back(ch);
    }

    return chars;
}