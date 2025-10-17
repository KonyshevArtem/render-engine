#ifndef RENDER_ENGINE_UI_MANAGER_H
#define RENDER_ENGINE_UI_MANAGER_H

#include "vector2/vector2.h"

#include <memory>
#include <string>
#include <vector>

class UIElement;
class UIImage;
class Texture2D;

class UIManager
{
public:
    static std::shared_ptr<UIImage> CreateImage(const Vector2& position, const Vector2& size, const std::shared_ptr<Texture2D> image);

    static std::vector<std::shared_ptr<UIElement>> Elements;
};

#endif //RENDER_ENGINE_UI_MANAGER_H
