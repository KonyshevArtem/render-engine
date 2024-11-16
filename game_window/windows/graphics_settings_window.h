#ifndef RENDER_ENGINE_GRAPHICS_SETTINGS_WINDOW_H
#define RENDER_ENGINE_GRAPHICS_SETTINGS_WINDOW_H

#ifdef ENABLE_IMGUI

#include "base_window.h"

class GraphicsSettingsWindow : public BaseWindow
{
public:
    GraphicsSettingsWindow();
    ~GraphicsSettingsWindow() override = default;

protected:
    void DrawInternal() override;
};

#endif

#endif //RENDER_ENGINE_GRAPHICS_SETTINGS_WINDOW_H
