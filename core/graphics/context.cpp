#include "context.h"
#include "../camera/camera.h"
#include "../scene/scene.h"

Context::Context()
{
    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return;

    auto scene = Scene::Current;

    AmbientLight     = scene->AmbientLight;
    Lights           = scene->Lights;
    GameObjects      = scene->GameObjects;
    ViewMatrix       = Camera::Current->GetViewMatrix();
    ProjectionMatrix = Camera::Current->GetProjectionMatrix();
}
