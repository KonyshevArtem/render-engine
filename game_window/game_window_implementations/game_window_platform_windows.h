#if RENDER_ENGINE_WINDOWS

#ifndef RENDER_ENGINE_GAME_WINDOW_PLATFORM_WINDOWS_H
#define RENDER_ENGINE_GAME_WINDOW_PLATFORM_WINDOWS_H

#include "game_window.h"

class GameWindowPlatformWindows : public GameWindow
{
public:
    explicit GameWindowPlatformWindows(void* viewPtr);
    ~GameWindowPlatformWindows() override;

    void TickMainLoop(int width, int height) override;
};


#endif //RENDER_ENGINE_GAME_WINDOW_PLATFORM_WINDOWS_H

#endif