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
    UIText(const Vector2& position, const Vector2& size, const std::string& text, const std::shared_ptr<Font>& font);

    void SetText(const std::string& text);
    void PrepareText();

    const std::shared_ptr<Mesh> GetMesh() const;
    const std::shared_ptr<Texture> GetFontAtlas() const;

private:
    bool m_Dirty;
    Vector2 m_PrevSize;
    std::string m_Text;
    std::shared_ptr<Font> m_Font;
    std::shared_ptr<Mesh> m_Mesh;
};

#endif //RENDER_ENGINE_UI_TEXT_H
