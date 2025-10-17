#include "ui_image.h"

UIImage::UIImage(const Vector2 &position, const Vector2& size, const std::shared_ptr<Texture2D> image) :
    UIElement(position, size),
    Color(1, 1, 1, 1),
    Image(image)
{
}