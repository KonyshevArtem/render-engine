#include "ui_manager.h"
#include "ui_image.h"
#include "ui_text.h"
#include "font/font.h"
#include "resources/resources.h"
#include "graphics/graphics.h"

std::shared_ptr<Font> UIManager::s_Font;
std::shared_ptr<UIElement> UIManager::s_Root;

Vector2 UIManager::s_ReferenceSize(0, 0);

void UIManager::Initialize(const Vector2& referenceSize)
{
    s_ReferenceSize = referenceSize;
    s_Root = std::make_shared<UIElement>(Vector2(0, 0), Vector2(0, 0));
}

void UIManager::Update()
{
}

std::shared_ptr<UIImage> UIManager::CreateImage(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image)
{
    std::shared_ptr<UIImage> uiImage = std::make_shared<UIImage>(position, size, image);
    uiImage->SetParent(parent != nullptr ? parent : s_Root);
    return uiImage;
}

std::shared_ptr<UIText> UIManager::CreateText(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::string& text, uint16_t fontSize)
{
    if (!s_Font)
        s_Font = Resources::Load<Font>("core_resources/fonts/Inter.ttf");

    std::shared_ptr<UIText> uiText = std::make_shared<UIText>(position, size, text, s_Font);
    uiText->SetParent(parent != nullptr ? parent : s_Root);
    uiText->SetFontSize(fontSize);
    return uiText;
}

void UIManager::CollectElements(std::vector<UIElement*>& outElements)
{
    CollectElements(*s_Root, outElements);
}

void UIManager::CollectElements(UIElement& element, std::vector<UIElement *>& outElements)
{
    outElements.push_back(&element);
    for (std::shared_ptr<UIElement>& child : element.m_Children)
    {
        if (child)
        {
            child->m_GlobalPosition = element.m_GlobalPosition + child->Position;
            CollectElements(*child, outElements);
        }
    }
}
