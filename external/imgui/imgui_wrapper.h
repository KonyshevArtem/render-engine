#ifndef RENDER_ENGINE_IMGUI_WRAPPER_H
#define RENDER_ENGINE_IMGUI_WRAPPER_H

#include <functional>

namespace ImGuiWrapper
{
    void Init(const std::function<void(void*)>& fillImGuiData);
    void Shutdown();
    void NewFrame();
    void Render();
}

#endif //RENDER_ENGINE_IMGUI_WRAPPER_H
