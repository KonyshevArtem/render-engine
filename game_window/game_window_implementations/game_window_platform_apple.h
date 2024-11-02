#if RENDER_ENGINE_APPLE

#ifndef RENDER_ENGINE_GAME_WINDOW_PLATFORM_APPLE_H
#define RENDER_ENGINE_GAME_WINDOW_PLATFORM_APPLE_H

#include "game_window.h"

class GameWindowPlatformApple : public GameWindow
{
public:
    explicit GameWindowPlatformApple();
    ~GameWindowPlatformApple() override;

    void TickMainLoop(int width, int height) override;
};


#endif //RENDER_ENGINE_GAME_WINDOW_PLATFORM_APPLE_H

#endif
