#ifndef RENDER_ENGINE_UI_IMAGE_H
#define RENDER_ENGINE_UI_IMAGE_H

#include "ui_element.h"
#include "vector4/vector4.h"

#include <memory>

class Texture2D;

class UIImage : public UIElement
{
public:
    static std::shared_ptr<UIImage> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image);

    Vector4 Color;
    std::shared_ptr<Texture2D> Image;

private:
    UIImage(const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image);
};

#endif //RENDER_ENGINE_UI_IMAGE_H
