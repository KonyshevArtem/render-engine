#ifndef RENDER_ENGINE_UI_TEXT_FIELD_H
#define RENDER_ENGINE_UI_TEXT_FIELD_H

#include "ui_element.h"

#include <functional>
#include <string>

class UIText;
class UIImage;

class UITextField : public UIElement
{
public:
    UITextField(const Vector2& position, const Vector2& size);

    std::function<void(const std::string&)> OnFinish;

protected:
    void HandleEvent(UIEventInfo& eventInfo) override;
    void LoseFocus() override;

private:
    std::shared_ptr<UIText> m_Text;
    std::shared_ptr<UIImage> m_BackgroundImage;
    std::shared_ptr<UIImage> m_CursorImage;

    int m_CursorPosition;
    bool m_IsCursorActive;

    void SetCursorActive(bool isActive);
    void MoveCursor(int offset);
    void UpdateText(const std::function<void(std::string&)>& updateFunc, int cursorOffset);

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_TEXT_FIELD_H
