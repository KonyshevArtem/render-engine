#include <cmath>

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "utils/utils.h"

#include "core/camera/camera.h"
#include "core/graphics/graphics.h"
#include "core/input/input.h"
#include "core/time/time.h" // NOLINT(modernize-deprecated-headers)
#include "scenes/test_scene.h"

using namespace std;

void display()
{
    Time::Update();
    Input::Update();
    if (Input::IsKeyDown('q'))
    {
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }

    Scene::Update();
    Graphics::Render();
}

void reshape(int width, int height)
{
    Graphics::Reshape(width, height);
}

int main(int argc, char **argv)
{
    Graphics::Init(argc, argv);
    Input::Init();
    Camera::Init(120, 0.5f, 100);

    TestScene::Load();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
