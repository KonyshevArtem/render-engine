#ifndef RENDER_ENGINE_UI_ELEMENT_H
#define RENDER_ENGINE_UI_ELEMENT_H

#include "vector2/vector2.h"

#include <vector>
#include <memory>

struct UIEventInfo;

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
    static std::shared_ptr<UIElement> Create(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size);

    UIElement(const Vector2& position, const Vector2& size);

    Vector2 Position;
    Vector2 Size;
    bool Active;

    std::shared_ptr<UIElement> GetParent() const;
    void SetParent(const std::shared_ptr<UIElement>& parent);
    void Destroy();

    inline Vector2 GetGlobalPosition() const
    {
        return m_GlobalPosition;
    }

    virtual void Update() {}

protected:
    std::vector<std::shared_ptr<UIElement>> m_Children;

    virtual void HandleEvent(UIEventInfo& eventInfo){}
    virtual void OnGainFocus(){}
    virtual void OnLoseFocus(){}
    virtual void OnLoseHover(){}

    virtual void AddChild(const std::shared_ptr<UIElement>& child);

private:
    std::weak_ptr<UIElement> m_Parent;
    Vector2 m_GlobalPosition;

    void RemoveChild(const std::shared_ptr<UIElement>& child);

    friend class UIManager;
};

#endif //RENDER_ENGINE_UI_ELEMENT_H
