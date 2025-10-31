#ifndef RENDER_ENGINE_UI_MANAGER_H
#define RENDER_ENGINE_UI_MANAGER_H

#include "vector2/vector2.h"

#include <memory>
#include <string>
#include <vector>

class UIElement;
class UIImage;
class UIText;
class Texture2D;
class Font;

class UIManager
{
public:
    static std::shared_ptr<UIImage> CreateImage(const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image);
    static std::shared_ptr<UIText> CreateText(const Vector2& position, const Vector2& size, const std::string& text);

    static std::vector<std::shared_ptr<UIElement>> Elements;

private:
    static std::shared_ptr<Font> s_Font;
};

#endif //RENDER_ENGINE_UI_MANAGER_H
