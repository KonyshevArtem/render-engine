#ifndef RENDER_ENGINE_TOP_MENU_BAR_H
#define RENDER_ENGINE_TOP_MENU_BAR_H

#ifdef ENABLE_IMGUI

#include <functional>

namespace TopMenuBar
{
    void Draw(std::function<void()> closeWindow);
}

#endif

#endif //RENDER_ENGINE_TOP_MENU_BAR_H
