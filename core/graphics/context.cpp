#include "context.h"
#include "camera/camera.h"
#include "gameObject/gameObject.h"
#include "scene/scene.h"

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

    for (auto it = Scene::Current->cbegin(); it != Scene::Current->cend(); it++)
        CollectRenderers(*it);

    for (const auto &light: scene->Lights)
    {
        if (light != nullptr)
            Lights.push_back(light.get());
    }
}

void Context::CollectRenderers(const std::shared_ptr<GameObject> &_gameObject)
{
    if (!_gameObject)
        return;

    if (_gameObject->Renderer)
        Renderers.push_back(_gameObject->Renderer.get());

    for (auto it = _gameObject->Children.cbegin(); it != _gameObject->Children.cend(); it++)
        CollectRenderers(*it);
}