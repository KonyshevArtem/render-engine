#include "ui_button.h"
#include "ui_image.h"
#include "ui_text.h"
#include "ui_manager.h"

UIButton::UIButton(const Vector2& position, const Vector2& size) :
    UIElement(position, size)
{
}

void UIButton::SetImageColor(const Vector4& color)
{
    if (!m_Image.expired())
        m_Image.lock()->Color = color;
}
