#ifndef RENDER_ENGINE_UI_MANAGER_H
#define RENDER_ENGINE_UI_MANAGER_H

#include "vector2/vector2.h"

#include <memory>
#include <string>
#include <vector>

class UIElement;
class UIButton;
class UITextField;
class Texture2D;
struct UIEventInfo;

class UIManager
{
public:
    static void Initialize(float referenceHeight);
    static void Update();
    static void DestroyUI();
    static void ResetFocus();

    static inline const Vector2& GetReferenceSize()
    {
        return s_ReferenceSize;
    }

    static inline const std::vector<UIElement*> GetElements()
    {
        return s_Elements;
    }

    static std::shared_ptr<UIElement> GetRoot();

private:
    static std::shared_ptr<UIElement> s_Root;
    static std::vector<UIElement*> s_Elements;
    static std::shared_ptr<UIElement> s_FocusedElement;
    static std::shared_ptr<UIElement> s_HoveredElement;

    static Vector2 s_ReferenceSize;

    static void CollectElements(UIElement& element);
    static std::shared_ptr<UIElement> HandleEvent(UIEventInfo& eventInfo, std::shared_ptr<UIElement>& element);

    static void ChangeFocus(const std::shared_ptr<UIElement>& newFocusedElement);
};

#endif //RENDER_ENGINE_UI_MANAGER_H
