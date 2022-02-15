#include "context.h"
#include "../camera/camera.h"
#include "../gameObject/gameObject.h"
#include "../scene/scene.h"

Context::Context()
{
    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return;

    const auto &scene = Scene::Current;

    AmbientLight     = scene->AmbientLight;
    Skybox           = scene->Skybox.get();
    ViewMatrix       = Camera::Current->GetViewMatrix();
    ProjectionMatrix = Camera::Current->GetProjectionMatrix();
    ShadowDistance   = Camera::Current->GetShadowDistance();

    for (const auto &go: scene->GameObjects)
    {
        if (go != nullptr && go->Renderer != nullptr)
            Renderers.push_back(go->Renderer.get());
    }

    for (const auto &light: scene->Lights)
    {
        if (light != nullptr)
            Lights.push_back(light.get());
    }
}
