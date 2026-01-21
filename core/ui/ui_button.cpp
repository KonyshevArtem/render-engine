#include "ui_button.h"
#include "ui_image.h"
#include "ui_text.h"
#include "ui_manager.h"
#include "ui_event_info.h"

std::shared_ptr<UIButton> UIButton::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::wstring& text, uint16_t fontSize, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIButton> uiButton = std::shared_ptr<UIButton>(new UIButton(position, size));
    uiButton->SetParent(parent);

    std::shared_ptr<UIImage> uiImage = UIImage::Create(uiButton, Vector2(0, 0), size, image);
    std::shared_ptr<UIText> uiText = UIText::Create(uiButton, Vector2(0, 0), size, text, fontSize);
    uiText->SetHorizontalAlignment(UIText::HorizontalAlignment::MIDDLE);
    uiText->SetVerticalAlignment(UIText::VerticalAlignment::MIDDLE);

    uiButton->m_Image = uiImage;
    uiButton->m_Text = uiText;

    return uiButton;
}

UIButton::UIButton(const Vector2& position, const Vector2& size) :
    UIElement(position, size),
    m_ImageColor(Vector4(1, 1, 1, 1)),
    m_IsHovered(false),
    m_IsPressed(false)
{
}

void UIButton::SetImageColor(const Vector4& color)
{
    m_ImageColor = color;
    SetImageColor_Internal(m_ImageColor, 1);
}

void UIButton::HandleEvent(UIEventInfo& eventInfo)
{
    const float k_HoverColorMultiplier = 0.8f;
    const float k_PressColorMultiplier = 0.6f;

    if (eventInfo.Type == UIEventType::POINTER_DOWN)
    {
        if (!m_IsPressed)
        {
            m_IsPressed = true;
            SetImageColor_Internal(m_ImageColor, k_PressColorMultiplier);
        }
        eventInfo.Consumed = true;
    }

    if (eventInfo.Type == UIEventType::POINTER_UP)
    {
        if (m_IsPressed)
        {
            SetImageColor(m_ImageColor);
            m_IsHovered = false;
            m_IsPressed = false;

            if (OnPress)
                OnPress();
        }
        eventInfo.Consumed = true;
    }

    if (eventInfo.Type == UIEventType::HOVER)
    {
        if (!m_IsHovered)
        {
            m_IsHovered = true;
            SetImageColor_Internal(m_ImageColor, m_IsPressed ? k_PressColorMultiplier : k_HoverColorMultiplier);
        }
        eventInfo.Consumed = true;
    }
}

void UIButton::LoseFocus()
{
    if (m_IsPressed)
    {
        SetImageColor(m_ImageColor);
        m_IsPressed = false;
    }
}

void UIButton::LoseHover()
{
    if (m_IsHovered)
    {
        SetImageColor(m_ImageColor);
        m_IsHovered = false;
    }
}

void UIButton::SetImageColor_Internal(const Vector4& color, float colorMultiplier)
{
    if (!m_Image.expired())
        m_Image.lock()->Color = Vector4(color.x * colorMultiplier, color.y * colorMultiplier, color.z * colorMultiplier, color.w);
}
