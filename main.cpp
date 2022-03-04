#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h> // include glew before qt includes gl.h
#endif

#include "core_debug/debug.h"
#include "game_window.h"
#include "graphics/graphics.h"
#include "input/input.h"
#include "scenes/test_scene.h"
#include "editor_gui/editor_window.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)

#include <QApplication>

void display()
{
    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render();

    Input::CleanUp();
}

int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    #if OPENGL_STUDY_EDITOR
    EditorWindow editor;
    editor.resize(1024, 720);
    editor.show();
    #endif

    GameWindow window(Graphics::OPENGL_MAJOR_VERSION,
                      Graphics::OPENGL_MINOR_VERSION,
                      Graphics::Init,
                      Graphics::Reshape,
                      display,
                      Input::HandleKeyboardInput,
                      Input::HandleMouseMove);

    window.resize(1024, 720);
    window.show();

    Time::Init();
    Debug::Init();

    TestScene::Load();

    return application.exec();
}
