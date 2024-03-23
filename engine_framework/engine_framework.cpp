#include "engine_framework.h"
#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "../scenes/test_scene.h"
#include "../scenes/pbr_demo.h"
#include "../scenes/shadows_demo.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

GameWindow* window = nullptr;

void display(int width, int height)
{
    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render(width, height);

    Input::CleanUp();
}

void EngineFramework::Initialize(void* viewPtr)
{
    window = GameWindow::Create(viewPtr, display, Input::HandleKeyboardInput, Input::HandleMouseMove);

    Graphics::Init();
    Time::Init();

    TestScene::Load();
    //PBRDemo::Load();
    //ShadowsDemo::Load();
}

void EngineFramework::TickMainLoop(int width, int height)
{
    if (window)
    {
        window->TickMainLoop(width, height);
    }
}

bool EngineFramework::ShouldCloseWindow()
{
    return window && window->ShouldCloseWindow();
}

void EngineFramework::Shutdown()
{
    delete window;

    Graphics::Shutdown();
}

void EngineFramework::ProcessMouseMove(float x, float y)
{
    if (window)
    {
        window->ProcessMouseMove(x, y);
    }
}

void EngineFramework::ProcessKeyPress(char key, bool pressed)
{
    if (window)
    {
        window->ProcessKeyPress(key, pressed);
    }
}
