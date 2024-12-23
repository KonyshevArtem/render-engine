#ifndef RENDER_ENGINE_IMGUI_WRAPPER_H
#define RENDER_ENGINE_IMGUI_WRAPPER_H

namespace ImGuiWrapper
{
    void Init(void* data);
    void Shutdown();
    void NewFrame(void* data);
    void Render(void* data);
}

#endif //RENDER_ENGINE_IMGUI_WRAPPER_H
