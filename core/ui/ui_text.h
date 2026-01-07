#ifndef RENDER_ENGINE_UI_TEXT_H
#define RENDER_ENGINE_UI_TEXT_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <memory>
#include <string>

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

    UIText(const Vector2& position, const Vector2& size, const std::string& text, const std::shared_ptr<Font>& font);

    void SetText(const std::string& text);
    void SetFontSize(uint16_t fontSize);
    void SetHorizontalAlignment(HorizontalAlignment alignment);
    void SetVerticalAlignment(VerticalAlignment alignment);

    inline const std::string& GetText() const
    {
        return m_Text;
    }

    void PrepareFont();
    void PrepareMesh();

    const std::shared_ptr<Mesh> GetMesh() const;
    const std::shared_ptr<Texture> GetFontAtlas() const;
    const std::shared_ptr<Font> GetFont() const;
    uint16_t GetFontSize() const;

    Vector4 Color;

private:
    bool m_Dirty;
    Vector2 m_PrevSize;
    uint16_t m_FontSize;
    uint16_t m_PrevFontSize;
    std::string m_Text;
    std::shared_ptr<Font> m_Font;
    std::shared_ptr<Mesh> m_Mesh;
    HorizontalAlignment m_HorizontalAlignment;
    VerticalAlignment m_VerticalAlignment;
};

#endif //RENDER_ENGINE_UI_TEXT_H
