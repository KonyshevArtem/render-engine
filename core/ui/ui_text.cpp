#include "ui_text.h"
#include "font/font.h"
#include "mesh/mesh.h"
#include "texture_2d_array/texture_2d_array.h"
#include "editor/profiler/profiler.h"

UIText::UIText(const Vector2 &position, const Vector2& size, const std::string& text, const std::shared_ptr<Font>& font) :
    UIElement(position, size),
    m_Dirty(true),
    m_Text(text),
    m_Font(font)
{
}

void UIText::SetText(const std::string& text)
{
    m_Dirty = true;
    m_Text = text;
}

void UIText::PrepareText()
{
    if (!m_Dirty && m_PrevSize == Size)
        return;

    Profiler::Marker _("UIText::PrepareText");

    std::vector<int> indices;
    std::vector<Vector3> positions;
    std::vector<Vector2> uvs;

    indices.reserve(m_Text.size() * 2 * 3);
    positions.reserve(m_Text.size() * 4);
    uvs.reserve(m_Text.size() * 4);

    const CommonBlock& common = m_Font->GetCommonBlock();

    uint32_t xPosition = 0;
    int32_t yPosition = Size.y;
    for (int i = 0; i < m_Text.size(); ++i)
    {
        char c = m_Text[i];
        const Char& ch = m_Font->GetChar(c);

        if (c == '\n' || c == '\r')
        {
            xPosition = 0;
            yPosition -= common.LineHeight;
            continue;
        }

        if (!isspace(c))
        {
            float charWidth  = static_cast<float>(ch.Width) / common.ScaleW;
            float charHeight = static_cast<float>(ch.Height) / common.ScaleH;

            Vector2 minUv = Vector2(static_cast<float>(ch.X) / common.ScaleW, static_cast<float>(ch.Y) / common.ScaleH);
            Vector2 maxUv = Vector2(minUv.x + charWidth, minUv.y + charHeight);

            Vector3 minPos = Vector3(xPosition, (yPosition - ch.Height - ch.YOffset), 0.5f);
            Vector3 maxPos = Vector3(minPos.x + ch.Width, minPos.y + ch.Height, 0.5f);

            const int32_t baseVertexIndex = positions.size();
            positions.push_back(minPos);
            positions.push_back({maxPos.x, minPos.y, 0.5f});
            positions.push_back(maxPos);
            positions.push_back({minPos.x, maxPos.y, 0.5f});

            indices.push_back(0 + baseVertexIndex);
            indices.push_back(2 + baseVertexIndex);
            indices.push_back(1 + baseVertexIndex);
            indices.push_back(0 + baseVertexIndex);
            indices.push_back(3 + baseVertexIndex);
            indices.push_back(2 + baseVertexIndex);

            uvs.push_back({minUv.x, maxUv.y});
            uvs.push_back(maxUv);
            uvs.push_back({maxUv.x, minUv.y});
            uvs.push_back(minUv);
        }

        const int16_t kerning = i < m_Text.size() - 1 ? m_Font->GetKerning(c, m_Text[i + 1]) : 0;
        xPosition += ch.XAdvance + kerning;
    }

    m_Mesh = std::make_shared<Mesh>(positions, std::vector<Vector3>(), indices, uvs, std::vector<Vector3>(), "TextMesh");

    m_Dirty = false;
    m_PrevSize = Size;
}

const std::shared_ptr<Mesh> UIText::GetMesh() const
{
    return m_Mesh;
}

const std::shared_ptr<Texture> UIText::GetFontAtlas() const
{
    return m_Font->GetAtlas();
}