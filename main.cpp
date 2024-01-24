#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h> // include glew before qt includes gl.h
#endif

#include "debug.h"
#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "scenes/test_scene.h"
#include "scenes/pbr_demo.h"
#include "scenes/shadows_demo.h"
//#include "editor_gui/editor_window.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

void display()
{
    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render();

    Input::CleanUp();
}

#ifdef OPENGL_STUDY_WINDOWS
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#elif OPENGL_STUDY_MACOS
int main(int __argc, char **__argv)
#endif
{
    // init debug first so that errors from initialization of other components will be intercepted
    Debug::Init();

//    #if OPENGL_STUDY_EDITOR
//    EditorWindow editor;
//    editor.resize(1024, 720);
//    editor.show();
//    #endif

    GameWindow window(1024, 720, Graphics::Reshape, display, Input::HandleKeyboardInput, Input::HandleMouseMove);

    Graphics::Init();
    Time::Init();

    TestScene::Load();
    //PBRDemo::Load();
    //ShadowsDemo::Load();

    window.BeginMainLoop();

    Graphics::Shutdown();

    return 0;
}
