#ifndef RENDER_ENGINE_UI_ELEMENT_H
#define RENDER_ENGINE_UI_ELEMENT_H

#include "vector2/vector2.h"

#include <vector>
#include <memory>

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
    UIElement(const Vector2& position, const Vector2& size);

    Vector2 Position;
    Vector2 Size;

    void SetParent(const std::shared_ptr<UIElement>& parent);
    void RemoveChild(const std::shared_ptr<UIElement>& child);

protected:
    virtual void Stub(){};

private:
    std::vector<std::shared_ptr<UIElement>> m_Children;
    std::weak_ptr<UIElement> m_Parent;

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_ELEMENT_H
