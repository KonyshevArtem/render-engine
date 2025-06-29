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
#include "imgui_wrapper.h"
#include "file_system/file_system.h"
#include "arguments.h"
#include "../scripts/game_components_register.h"
#include "core_components_register.h"

GameWindow* window = nullptr;

void display(int width, int height)
{
    Profiler::BeginNewFrame();
    Profiler::Marker marker("Process Frame");

    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render(width, height);

    Input::CleanUp();
}

void EngineFramework::Initialize(void* fileSystemData, void* graphicsBackendInitData, char** argv, int argc)
{
    Arguments::Init(argv, argc);

    CoreComponents::Register();
    GameComponents::Register();

    FileSystem::Init(fileSystemData);
    GraphicsBackend::Init(graphicsBackendInitData);
    ImGuiWrapper::Init();

    window = new GameWindow(display);

    Graphics::Init();
    Time::Init();

    TestScene::Load();
    //PBRDemo::Load();
    //ShadowsDemo::Load();

    GraphicsBackend::Current()->Flush();
    GraphicsBackend::Current()->Present();
}

void EngineFramework::TickMainLoop(int width, int height)
{
    if (window)
    {
        ImGuiWrapper::NewFrame();
        GraphicsBackend::Current()->InitNewFrame();

        window->TickMainLoop(width, height);
        GraphicsBackend::Current()->Flush();

        ImGuiWrapper::Render();

        GraphicsBackend::Current()->Present();
    }
}

bool EngineFramework::ShouldCloseWindow()
{
    return window && window->ShouldCloseWindow();
}

void EngineFramework::Shutdown()
{
    delete window;

    ImGuiWrapper::Shutdown();
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
