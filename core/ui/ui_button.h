#ifndef RENDER_ENGINE_UI_BUTTON_H
#define RENDER_ENGINE_UI_BUTTON_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <functional>
#include <string>

class UIText;
class UIImage;
class Texture2D;

class UIButton : public UIElement
{
public:
    static std::shared_ptr<UIButton> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::wstring& text, uint16_t fontSize, const std::shared_ptr<Texture2D> image);

    void SetImageColor(const Vector4& color);

    std::function<void()> OnPress;

protected:
    void HandleEvent(UIEventInfo& eventInfo) override;
    void LoseFocus() override;
    void LoseHover() override;

private:
    UIButton(const Vector2& position, const Vector2& size);

    std::weak_ptr<UIText> m_Text;
    std::weak_ptr<UIImage> m_Image;
    Vector4 m_ImageColor;
    bool m_IsHovered;
    bool m_IsPressed;

    void SetImageColor_Internal(const Vector4& color, float colorMultiplier);

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_BUTTON_H
