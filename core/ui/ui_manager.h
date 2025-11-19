#ifndef RENDER_ENGINE_UI_MANAGER_H
#define RENDER_ENGINE_UI_MANAGER_H

#include "vector2/vector2.h"

#include <memory>
#include <string>
#include <vector>

class UIElement;
class UIImage;
class UIText;
class UIButton;
class Texture2D;
class Font;
struct UIEventInfo;

class UIManager
{
public:
    static void Initialize(const Vector2& referenceSize);
    static void Update();
    static void DestroyUI();

    static inline const Vector2& GetReferenceSize()
    {
        return s_ReferenceSize;
    }

    static std::shared_ptr<UIImage> CreateImage(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image);
    static std::shared_ptr<UIText> CreateText(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::string& text, uint16_t fontSize);
    static std::shared_ptr<UIButton> CreateButton(std::shared_ptr<UIElement> parent, const Vector2& position, const Vector2& size, const std::string& text, uint16_t fontSize, const std::shared_ptr<Texture2D> image);

    static inline const std::vector<UIElement*> GetElements()
    {
        return s_Elements;
    }

private:
    static std::shared_ptr<Font> s_Font;
    static std::shared_ptr<UIElement> s_Root;
    static std::vector<UIElement*> s_Elements;
    static std::shared_ptr<UIElement> s_FocusedElement;
    static std::shared_ptr<UIElement> s_HoveredElement;

    static Vector2 s_ReferenceSize;

    static void CollectElements(UIElement& element);
    static std::shared_ptr<UIElement> HandleEvent(UIEventInfo& eventInfo, std::shared_ptr<UIElement>& element);
};

#endif //RENDER_ENGINE_UI_MANAGER_H
