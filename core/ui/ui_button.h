#ifndef RENDER_ENGINE_UI_BUTTON_H
#define RENDER_ENGINE_UI_BUTTON_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <functional>

class UIText;
class UIImage;

class UIButton : public UIElement
{
public:
    UIButton(const Vector2& position, const Vector2& size);

    void SetImageColor(const Vector4& color);

    std::function<void()> OnClick;

protected:
    void HandleEvent(UIEventInfo& eventInfo) override;

private:
    std::weak_ptr<UIText> m_Text;
    std::weak_ptr<UIImage> m_Image;

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_BUTTON_H
