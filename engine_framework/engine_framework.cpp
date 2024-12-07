#include "engine_framework.h"
#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "../scenes/test_scene.h"
#include "../scenes/pbr_demo.h"
#include "../scenes/shadows_demo.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "file_system/file_system.h"

GameWindow* window = nullptr;

void display(int width, int height)
{
    Profiler::BeginNewFrame();

    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render(width, height);

    Input::CleanUp();
}

void EngineFramework::Initialize(void* fileSystemData, void* graphicsBackendInitData, const char* graphicsBackend)
{
    FileSystem::Init(static_cast<FileSystem::FileSystemData*>(fileSystemData));
    GraphicsBackend::Init(graphicsBackendInitData, graphicsBackend);

    window = new GameWindow(display);

    Graphics::Init();
    Time::Init();

    TestScene::Load();
    //PBRDemo::Load();
    //ShadowsDemo::Load();
}

void EngineFramework::TickMainLoop(void* graphicsBackendFrameData, int width, int height)
{
    if (window)
    {
        GraphicsBackend::Current()->InitNewFrame(graphicsBackendFrameData);
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

void EngineFramework::ProcessMouseClick(int mouseButton, bool pressed)
{
    if (!window || !window->CaptureMouse())
    {
        Input::HandleMouseClick(static_cast<Input::MouseButton>(mouseButton), pressed);
    }
}

void EngineFramework::ProcessMouseMove(float x, float y)
{
    if (!window || !window->CaptureMouse())
    {
        Input::HandleMouseMove(x, y);
    }
}

void EngineFramework::ProcessKeyPress(char key, bool pressed)
{
    if (!window || !window->CaptureKeyboard())
    {
        Input::HandleKeyboardInput(key, pressed);
    }
}

void EngineFramework::ProcessTouchDown(unsigned long touchId, float x, float y)
{
    Input::HandleTouch(Input::TouchState::DOWN, touchId, x, y);
}

void EngineFramework::ProcessTouchMove(unsigned long touchId, float x, float y)
{
    Input::HandleTouch(Input::TouchState::MOVE, touchId, x, y);
}

void EngineFramework::ProcessTouchUp(unsigned long touchId)
{
    Input::HandleTouch(Input::TouchState::UP, touchId, 0, 0);
}
