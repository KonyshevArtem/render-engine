#include "ui_element.h"

UIElement::UIElement(const Vector2& position, const Vector2& size) :
    Position(position),
    Size(size)
{
}

void UIElement::SetParent(const std::shared_ptr<UIElement>& parent)
{
    std::shared_ptr<UIElement> thisPtr = shared_from_this();

    if (!m_Parent.expired())
    {
        std::vector<std::shared_ptr<UIElement>> children = m_Parent.lock()->m_Children;
        children.erase(std::remove_if(children.begin(), children.end(), [thisPtr](std::shared_ptr<UIElement>& child){return thisPtr == child;}), children.end());
    }

    m_Parent = parent;
    if (parent)
        parent->m_Children.push_back(thisPtr);
}