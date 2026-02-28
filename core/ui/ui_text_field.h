#ifndef RENDER_ENGINE_UI_TEXT_FIELD_H
#define RENDER_ENGINE_UI_TEXT_FIELD_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <functional>
#include <string>

class UIText;
class UIImage;
class Texture2D;
class UIMask;

class UITextField : public UIElement
{
public:
    static std::shared_ptr<UITextField> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, uint16_t fontSize, const std::shared_ptr<Texture2D> image);

    const std::wstring& GetText() const;
    void SetText(const std::wstring& text);

    void SetTextColor(const Vector4& color);
    void SetBackgroundColor(const Vector4& color);

    void Done();

    std::function<void(const std::wstring&)> OnFinish;

    void Update() override;

    bool KeepFocusOnDone;

protected:
    void HandleEvent(UIEventInfo& eventInfo) override;
    void OnGainFocus() override;
    void OnLoseFocus() override;

private:
    UITextField(const Vector2& position, const Vector2& size);

    std::shared_ptr<UIText> m_Text;
    std::shared_ptr<UIImage> m_BackgroundImage;
    std::shared_ptr<UIImage> m_CursorImage;
    std::shared_ptr<UIMask> m_Mask;

    int m_CursorPosition;
    bool m_IsCursorActive;

    void SetCursorActive(bool isActive);
    void MoveCursor(int offset);
    void UpdateText(const std::function<void(std::wstring&)>& updateFunc, int cursorOffset);

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_TEXT_FIELD_H
