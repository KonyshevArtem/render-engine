#ifndef RENDER_ENGINE_GAME_WINDOW_METAL_H
#define RENDER_ENGINE_GAME_WINDOW_METAL_H

#include "game_window.h"

namespace NS
{
    class AutoreleasePool;
    class Application;
}

class RenderEngineAppDelegate;
class RenderEngineViewDelegate;

class GameWindowMetal : public GameWindow
{
public:
    GameWindowMetal(int width, int height,
                    RenderHandler renderHandler,
                    KeyboardInputHandlerDelegate keyboardInputHandler,
                    MouseMoveHandlerDelegate mouseMoveHandler);

    ~GameWindowMetal() override;

    void BeginMainLoop() override;

protected:
    void SetCloseFlag() override;

private:
    NS::AutoreleasePool *m_AutoreleasePool;
    NS::Application *m_Application;
    RenderEngineAppDelegate *m_AppDelegate;
};


#endif //RENDER_ENGINE_GAME_WINDOW_METAL_H
