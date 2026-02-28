#include "engine_framework.h"
#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "scene/scene.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "imgui_wrapper.h"
#include "file_system/file_system.h"
#include "arguments.h"
#include "../scripts/game_components_register.h"
#include "core_components_register.h"
#include "resources/resources.h"
#include "worker/worker.h"
#include "ui/ui_manager.h"
#include "developer_console/developer_console.h"

GameWindow* window = nullptr;

void display(int width, int height)
{
    Profiler::Marker marker("Tick Main Loop");

    Time::Update();
    Input::Update();
    DeveloperConsole::Instance->Update();
    UIManager::Update();
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
    Worker::Init();

    window = new GameWindow(display);

    Graphics::Init();
    Time::Init();

#if RENDER_ENGINE_WINDOWS
    float uiHeight = 1080;
#else
    float uiHeight = 720;
#endif
    UIManager::Initialize(uiHeight);

    DeveloperConsole::Init();

    std::string scenePath = "core_resources/scenes/test_scene.scene";
    if (Arguments::Contains("-scene"))
        scenePath = Arguments::Get("-scene");
    Scene::Load(scenePath);

    GraphicsBackend::Current()->Flush();
    GraphicsBackend::Current()->Present();
}

void EngineFramework::TickMainLoop(int width, int height)
{
    if (window)
    {
        GraphicsBackend::Current()->IncrementFrameNumber();

        Profiler::BeginNewFrame();
        Profiler::Marker _("EngineFramework::TickMainLoop");

        {
            Profiler::Marker _("ImGuiWrapper::NewFrame");
            ImGuiWrapper::NewFrame();
        }

        {
            Profiler::Marker _("GraphicsBackend::InitNewFrame");
            GraphicsBackend::Current()->InitNewFrame();
        }

        window->TickMainLoop(width, height);

        {
            Profiler::Marker _("GraphicsBackend::Flush");
            GraphicsBackend::Current()->Flush();
        }

        {
            Profiler::Marker _("ImGuiWrapper::Render");
            Profiler::GPUMarker gpuMarker("ImGuiWrapper::Render");
            ImGuiWrapper::Render();
        }

        {
            Profiler::Marker _("GraphicsBackend::Present");
            GraphicsBackend::Current()->Present();
        }
    }
}

bool EngineFramework::ShouldCloseWindow()
{
    return window && window->ShouldCloseWindow();
}

void EngineFramework::Shutdown()
{
    delete window;

    Scene::Unload();
    Resources::UnloadAllResources();

    Worker::Shutdown();
    ImGuiWrapper::Shutdown();
    Graphics::Shutdown();
}

void EngineFramework::ProcessMouseClick(int mouseButton, bool pressed)
{
    if (!window || !window->CaptureMouse())
        Input::HandleMouseClick(static_cast<Input::MouseButton>(mouseButton), pressed);
}

void EngineFramework::ProcessMouseMove(float x, float y)
{
    if (!window || !window->CaptureMouse())
        Input::HandleMouseMove(x, y);
}

void EngineFramework::ProcessKeyPress(char key, bool pressed)
{
    if (!window || !window->CaptureKeyboard())
        Input::HandleKeyboardInput(key, pressed);
}

void EngineFramework::ProcessCharInput(wchar_t ch)
{
    if (!window || !window->CaptureKeyboard())
        Input::HandleCharInput(ch);
}

void EngineFramework::ProcessSpecialKey(int keyId, bool pressed)
{
    if (!window || !window->CaptureKeyboard())
        Input::HandleSpecialKeyInput(keyId, pressed);
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
