#ifndef RENDER_ENGINE_UI_MASK_H
#define RENDER_ENGINE_UI_MASK_H

#include "ui_element.h"

class UIMask : public UIElement
{
public:
    static std::shared_ptr<UIMask> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size);

protected:
    void AddChild(const std::shared_ptr<UIElement>& child) override;

private:
    UIMask(const Vector2& position, const Vector2& size);

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_MASK_H
