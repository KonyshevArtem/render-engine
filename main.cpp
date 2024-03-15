#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "scenes/test_scene.h"
#include "scenes/pbr_demo.h"
#include "scenes/shadows_demo.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

void display(int width, int height)
{
    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render(width, height);

    Input::CleanUp();
}

int main(int argc, char **argv)
{
    auto *window = GameWindow::Create(1920, 1080, display, Input::HandleKeyboardInput, Input::HandleMouseMove);

    Graphics::Init();
    Time::Init();

    TestScene::Load();
    //PBRDemo::Load();
    //ShadowsDemo::Load();

    window->BeginMainLoop();

    Graphics::Shutdown();

    delete window;

    return 0;
}
