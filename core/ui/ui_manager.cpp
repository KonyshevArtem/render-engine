#include "ui_manager.h"
#include "ui_image.h"
#include "ui_text.h"
#include "font/font.h"
#include "resources/resources.h"

std::vector<std::shared_ptr<UIElement>> UIManager::Elements;
std::shared_ptr<Font> UIManager::s_Font;

std::shared_ptr<UIImage> UIManager::CreateImage(const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIImage> uiImage = std::make_shared<UIImage>(position, size, image);
    Elements.push_back(uiImage);
    return uiImage;
}

std::shared_ptr<UIText> UIManager::CreateText(const Vector2& position, const Vector2& size, const std::string& text)
{
    if (!s_Font)
        s_Font = Resources::Load<Font>("core_resources/fonts/Inter.ttf");

    std::shared_ptr<UIText> uiImage = std::make_shared<UIText>(position, size, text, s_Font);
    Elements.push_back(uiImage);
    return uiImage;
}
