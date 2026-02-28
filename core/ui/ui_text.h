#ifndef RENDER_ENGINE_UI_TEXT_H
#define RENDER_ENGINE_UI_TEXT_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <memory>
#include <string>
#include <shared_mutex>

class Font;
class Mesh;
class Texture;

class UIText : public UIElement
{
public:
    enum class HorizontalAlignment
    {
        LEFT,
        MIDDLE,
        RIGHT
    };

    enum class VerticalAlignment
    {
        TOP,
        MIDDLE,
        BOTTOM
    };

    static std::shared_ptr<UIText> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::wstring& text, uint16_t fontSize);

    void SetText(const std::wstring& text);
    void SetFontSize(uint16_t fontSize);
    void SetHorizontalAlignment(HorizontalAlignment alignment);
    void SetVerticalAlignment(VerticalAlignment alignment);

    inline const std::wstring& GetText() const
    {
        return m_Text;
    }

    void PrepareFont();
    void PrepareMesh();

    const std::shared_ptr<Mesh> GetMesh() const;
    const std::shared_ptr<Texture> GetFontAtlas() const;
    const std::shared_ptr<Font> GetFont() const;
    uint16_t GetFontSize() const;
    Vector2 GetTextSize() const;

    Vector4 Color;

private:
    UIText(const Vector2& position, const Vector2& size, const std::wstring& text, const std::shared_ptr<Font>& font);

    static std::shared_ptr<Font> s_Font;

    bool m_Dirty;
    Vector2 m_PrevSize;
    uint16_t m_FontSize;
    uint16_t m_PrevFontSize;
    uint32_t m_PrevFontRevision;
    std::wstring m_Text;
    std::shared_ptr<Font> m_Font;
    std::shared_ptr<Mesh> m_Mesh;
    HorizontalAlignment m_HorizontalAlignment;
    VerticalAlignment m_VerticalAlignment;
    std::shared_mutex m_TextMutex;
    Vector2 m_TextSize;
};

#endif //RENDER_ENGINE_UI_TEXT_H
