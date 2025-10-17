#include "ui_manager.h"
#include "ui_image.h"

std::vector<std::shared_ptr<UIElement>> UIManager::Elements;

std::shared_ptr<UIImage> UIManager::CreateImage(const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIImage> uiImage = std::make_shared<UIImage>(position, size, image);
    Elements.push_back(uiImage);
    return uiImage;
}
