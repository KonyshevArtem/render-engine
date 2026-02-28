#include "ui_element.h"
#include "ui_manager.h"

std::shared_ptr<UIElement> UIElement::Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size)
{
    std::shared_ptr<UIElement> element = std::make_shared<UIElement>(position, size);
    element->SetParent(parent);

    return element;
}

UIElement::UIElement(const Vector2& position, const Vector2& size) :
    Position(position),
    Size(size),
    Active(true),
    m_GlobalPosition(Vector2(0, 0))
{
}

std::shared_ptr<UIElement> UIElement::GetParent() const
{
    return m_Parent.expired() ? nullptr : m_Parent.lock();
}

void UIElement::SetParent(const std::shared_ptr<UIElement>& parent)
{
    std::shared_ptr<UIElement> thisPtr = shared_from_this();

    if (!m_Parent.expired())
        m_Parent.lock()->RemoveChild(thisPtr);

    std::shared_ptr<UIElement> p = parent ? parent : UIManager::GetSceneUIRoot();

    m_Parent = p;
    if (p)
        p->AddChild(thisPtr);
}

void UIElement::AddChild(const std::shared_ptr<UIElement>& child)
{
    m_Children.push_back(child);
}

void UIElement::RemoveChild(const std::shared_ptr<UIElement>& child)
{
    m_Children.erase(std::remove_if(m_Children.begin(), m_Children.end(), [child](std::shared_ptr<UIElement>& ch){return child == ch;}), m_Children.end());
}

void UIElement::Destroy()
{
    if (!m_Parent.expired())
        m_Parent.lock()->RemoveChild(shared_from_this());
}
