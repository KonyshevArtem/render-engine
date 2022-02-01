#ifdef OPENGL_STUDY_WINDOWS
#include <GL/freeglut.h>
#elif OPENGL_STUDY_MACOS
#include "GLUT/glut.h"
#endif
#include "utils/utils.h"

#include "core/core_debug/debug.h"
#include "core/camera/camera.h"
#include "core/graphics/graphics.h"
#include "core/input/input.h"
#include "core/time/time.h" // NOLINT(modernize-deprecated-headers)
#include "scenes/test_scene.h"

void display()
{
    Time::Update();
    Input::Update();
    Scene::Update();

    Graphics::Render();

    Input::CleanUp();
}

void reshape(int _width, int _height)
{
    Graphics::Reshape(_width, _height);
}

#ifdef OPENGL_STUDY_WINDOWS
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#elif OPENGL_STUDY_MACOS
int main(int __argc, char** __argv)
#endif
{
    Debug::Init();
    Graphics::Init(__argc, __argv);
    Input::Init();
    Camera::Init(120, 0.5f, 100);

    TestScene::Load();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}