#ifndef RENDER_ENGINE_UI_MASK_STENCIL_H
#define RENDER_ENGINE_UI_MASK_STENCIL_H

#include "ui_element.h"

class Texture2D;

class UIMaskStencil : public UIElement
{
public:
    static std::shared_ptr<UIMaskStencil> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, bool open);

    bool Open;

private:
    UIMaskStencil(const Vector2& position, const Vector2& size, bool open);
};

#endif //RENDER_ENGINE_UI_MASK_STENCIL_H
