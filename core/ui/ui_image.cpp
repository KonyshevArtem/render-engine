#include "ui_image.h"

std::shared_ptr<UIImage> UIImage::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIImage> uiImage = std::shared_ptr<UIImage>(new UIImage(position, size, image));
    uiImage->SetParent(parent);
    return uiImage;
}

UIImage::UIImage(const Vector2 &position, const Vector2& size, const std::shared_ptr<Texture2D> image) :
    UIElement(position, size),
    Color(1, 1, 1, 1),
    Image(image)
{
}