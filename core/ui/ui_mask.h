#ifndef RENDER_ENGINE_UI_MASK_H
#define RENDER_ENGINE_UI_MASK_H

#include "ui_element.h"

class UIMaskStencil;

class UIMask : public UIElement
{
public:
    static std::shared_ptr<UIMask> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size);

    void Update() override;

protected:
    void AddChild(const std::shared_ptr<UIElement>& child) override;

private:
    UIMask(const Vector2& position, const Vector2& size);

    std::shared_ptr<UIMaskStencil> m_MaskStencils[2];

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_MASK_H
