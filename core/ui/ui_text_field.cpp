#include "ui_text_field.h"
#include "ui_event_info.h"
#include "ui_text.h"
#include "ui_image.h"
#include "ui_native_keyboard.h"
#include "ui_mask.h"
#include "font/font.h"
#include "texture_2d/texture_2d.h"

#include <algorithm>

std::shared_ptr<UITextField> UITextField::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, uint16_t fontSize, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UITextField> uiTextField = std::shared_ptr<UITextField>(new UITextField(position, size));
    uiTextField->SetParent(parent);

    std::shared_ptr<UIMask> mask = UIMask::Create(uiTextField, Vector2(0, 0), size);
    std::shared_ptr<UIImage> backgroundImage = UIImage::Create(mask, Vector2(0, 0), size, image);
    std::shared_ptr<UIText> uiText = UIText::Create(mask, Vector2(0, 0), size, L"", fontSize);
    std::shared_ptr<UIImage> cursorImage = UIImage::Create(mask, Vector2(0, 0), Vector2(1, size.y), Texture2D::White());

    const Vector4 defaultColor = Vector4(0.2f, 0.2f, 0.2f, 1);
    uiText->Color = defaultColor;
    cursorImage->Color = defaultColor;
    cursorImage->Active = false;

    uiTextField->m_BackgroundImage = backgroundImage;
    uiTextField->m_CursorImage = cursorImage;
    uiTextField->m_Text = uiText;

    return uiTextField;
}

UITextField::UITextField(const Vector2& position, const Vector2& size) :
    UIElement(position, size),
    m_CursorPosition(0),
    m_IsCursorActive(false)
{
}

const std::wstring& UITextField::GetText() const
{
    return m_Text->GetText();
}

void UITextField::SetText(const std::wstring& text)
{
    m_Text->SetText(text);
}

void UITextField::Done()
{
    if (OnFinish)
        OnFinish(m_Text->GetText());
}

void UITextField::HandleEvent(UIEventInfo& eventInfo)
{
    if (eventInfo.Type == UIEventType::POINTER_DOWN || eventInfo.Type == UIEventType::POINTER_UP)
    {
        UINativeKeyboard::ShowKeyboard(std::static_pointer_cast<UITextField>(shared_from_this()));
        SetCursorActive(true);
        eventInfo.Consumed = true;
    }

    if (eventInfo.Type == UIEventType::SPECIAL_KEY)
    {
        const Input::KeyboardKeyState& keyState = eventInfo.KeyState;
        switch (keyState.SpecialKey)
        {
            case Input::SpecialKey::LEFT:
            {
                if (keyState.IsDown())
                {
                    MoveCursor(-1);
                    eventInfo.Consumed = true;
                }
                break;
            }
            case Input::SpecialKey::RIGHT:
                if (keyState.IsDown())
                {
                    MoveCursor(1);
                    eventInfo.Consumed = true;
                }
                break;
            case Input::SpecialKey::DELETE:
                if (keyState.IsDown() && !m_Text->GetText().empty() && m_CursorPosition != m_Text->GetText().size())
                {
                    UpdateText([&](std::wstring& text){ text.erase(text.begin() + m_CursorPosition); }, 0);
                    eventInfo.Consumed = true;
                }
                break;
            default:
                break;
        }
    }

    if (eventInfo.Type == UIEventType::CHAR_INPUT)
    {
        if (eventInfo.KeyState.Char == '\b' || eventInfo.KeyState.Char == 127)
        {
            if (!m_Text->GetText().empty() && m_CursorPosition != 0)
            {
                UpdateText([&](std::wstring& text){ text.erase(text.begin() + m_CursorPosition - 1); }, -1);
                eventInfo.Consumed = true;
            }
        }
        else if (eventInfo.KeyState.Char == '\r')
        {
            Done();

            eventInfo.Consumed = true;
            eventInfo.LoseFocus = true;
        }
        else
        {
            UpdateText([&](std::wstring& text){ text.insert(text.begin() + m_CursorPosition, eventInfo.KeyState.Char); }, 1);
            eventInfo.Consumed = true;
        }
    }
}

void UITextField::LoseFocus()
{
    SetCursorActive(false);
}

void UITextField::SetCursorActive(bool isActive)
{
    m_CursorImage->Active = isActive;
    m_IsCursorActive = isActive;
}

void UITextField::MoveCursor(int offset)
{
    if (offset == 0)
        return;

    m_CursorPosition = std::clamp(m_CursorPosition + offset, 0, static_cast<int>(m_Text->GetText().size()));

    float textWidth;
    const std::wstring& text = m_Text->GetText();
    std::vector<Char> chars = m_Text->GetFont()->ShapeText(std::span<const wchar_t>(text.c_str(), m_CursorPosition), m_Text->GetFontSize(), textWidth);
    m_CursorImage->Position = Vector2(textWidth, 0);
}

void UITextField::UpdateText(const std::function<void(std::wstring &)>& updateFunc, int cursorOffset)
{
    std::wstring text = m_Text->GetText();
    updateFunc(text);
    m_Text->SetText(text);

    MoveCursor(cursorOffset);
}
