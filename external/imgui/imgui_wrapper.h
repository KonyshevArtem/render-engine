#ifndef RENDER_ENGINE_IMGUI_WRAPPER_H
#define RENDER_ENGINE_IMGUI_WRAPPER_H

#include <functional>

namespace ImGuiWrapper
{
    void Init(const char* graphicsBackend, const std::function<void(void*)>& fillImGuiData);
    void Shutdown();
    void NewFrame();
    void Render(const std::function<void(void*)>& fillImGuiData);
    void ProcessMessage(void* data);
}

#endif //RENDER_ENGINE_IMGUI_WRAPPER_H
