#include "font.h"
#include "Trex/Atlas.hpp"
#include "Trex/TextShaper.hpp"
#include "texture_2d/texture_2d.h"
#include "editor/profiler/profiler.h"

namespace Font_Local
{
    std::u32string Utf16ToUtf32(const std::span<const wchar_t>& utf16Text)
    {
        std::u32string out;
        out.reserve(utf16Text.size());

        for (size_t i = 0; i < utf16Text.size(); ++i)
        {
            wchar_t wc = utf16Text[i];

            if (wc >= 0xD800 && wc <= 0xDBFF)
            {
                wchar_t high = wc;
                wchar_t low  = utf16Text[++i];

                uint32_t cp =
                        0x10000 +
                        ((high - 0xD800) << 10) +
                        (low  - 0xDC00);

                out.push_back(cp);
            }
            else
                out.push_back(static_cast<char32_t>(wc));
        }

        return out;
    }
}

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

std::vector<Char> Font::ShapeText(const std::span<const wchar_t> text, uint16_t fontSize, float& outTextWidth)
{
    Profiler::Marker _("Font::ShapeText");

    auto it = m_TrexAtlas.find(fontSize);
    if (it == m_TrexAtlas.end())
        return std::vector<Char>();

    std::shared_ptr<Trex::Atlas> trexAtlas = it->second;

    Trex::TextShaper shaper(*trexAtlas);
    Trex::ShapedGlyphs glyphs;
    if (sizeof(wchar_t) == 4)
        glyphs = shaper.ShapeUtf32(std::span<const char32_t>(reinterpret_cast<const char32_t *>(text.data()), text.size()));
    else
    {
        std::u32string utf32String = Font_Local::Utf16ToUtf32(text);
        glyphs = shaper.ShapeUtf32(std::span<const char32_t>(utf32String.data(), utf32String.size()));
    }

    std::vector<Char> chars;
    chars.reserve(glyphs.size());

    outTextWidth = 0;
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

        outTextWidth += ch.XAdvance;
    }

    return chars;
}