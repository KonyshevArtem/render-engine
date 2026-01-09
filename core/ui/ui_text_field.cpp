#include "ui_text_field.h"
#include "ui_event_info.h"
#include "ui_text.h"
#include "ui_image.h"
#include "ui_native_keyboard.h"
#include "font/font.h"

#include <algorithm>

UITextField::UITextField(const Vector2& position, const Vector2& size) :
    UIElement(position, size),
    m_CursorPosition(0),
    m_IsCursorActive(false)
{
}

const std::string& UITextField::GetText() const
{
    return m_Text->GetText();
}

void UITextField::SetText(const std::string& text)
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
                    UpdateText([&](std::string& text){ text.erase(text.begin() + m_CursorPosition); }, 0);
                    eventInfo.Consumed = true;
                }
                break;
            default:
                break;
        }
    }

    if (eventInfo.Type == UIEventType::CHAR_INPUT)
    {
        if (eventInfo.KeyState.Char == '\b')
        {
            if (!m_Text->GetText().empty() && m_CursorPosition != 0)
            {
                UpdateText([&](std::string& text){ text.erase(text.begin() + m_CursorPosition - 1); }, -1);
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
            UpdateText([&](std::string& text){ text.insert(text.begin() + m_CursorPosition, eventInfo.KeyState.Char); }, 1);
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
    const std::string& text = m_Text->GetText();
    std::vector<Char> chars = m_Text->GetFont()->ShapeText(std::span<const char>(&text[0], m_CursorPosition), m_Text->GetFontSize(), textWidth);
    m_CursorImage->Position = Vector2(textWidth, 0);
}

void UITextField::UpdateText(const std::function<void(std::string &)>& updateFunc, int cursorOffset)
{
    std::string text = m_Text->GetText();
    updateFunc(text);
    m_Text->SetText(text);

    MoveCursor(cursorOffset);
}
