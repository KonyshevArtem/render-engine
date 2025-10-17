#ifndef RENDER_ENGINE_UI_ELEMENT_H
#define RENDER_ENGINE_UI_ELEMENT_H

#include "vector2/vector2.h"

class UIElement
{
public:
    UIElement(const Vector2& position, const Vector2& size);

    Vector2 Position;
    Vector2 Size;

protected:
    virtual void Stub(){};
};

#endif //RENDER_ENGINE_UI_ELEMENT_H
