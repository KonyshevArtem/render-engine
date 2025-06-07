#ifndef RENDER_ENGINE_IMGUI_WRAPPER_H
#define RENDER_ENGINE_IMGUI_WRAPPER_H

#include <functional>
#include <string>

namespace ImGuiWrapper
{
    void Init();
    void Shutdown();
    void NewFrame();
    void Render();
    void ProcessMessage(void* data);
}

#endif //RENDER_ENGINE_IMGUI_WRAPPER_H
