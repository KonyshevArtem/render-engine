#include "context.h"
#include "camera/camera.h"
#include "gameObject/gameObject.h"
#include "renderer/renderer.h"
#include "scene/scene.h"

Context::Context()
{
    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return;

    const auto &scene = Scene::Current;

    Skybox           = scene->Skybox;
    ViewMatrix       = Camera::Current->GetViewMatrix();
    ProjectionMatrix = Camera::Current->GetProjectionMatrix();
    ShadowDistance   = Camera::Current->GetShadowDistance();

    auto &gameObjects = Scene::Current->GetRootGameObjects();
    for (auto &go : gameObjects)
        CollectRenderers(go);

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

    const auto &renderer = _gameObject->Renderer;
    if (renderer)
    {
        Renderers.push_back(renderer);
        if (renderer->CastShadows)
        {
            if (ShadowCastersCount == 0)
            {
                ShadowCasterBounds = renderer->GetAABB();
            }
            else
            {
                ShadowCasterBounds = ShadowCasterBounds.Combine(renderer->GetAABB());
            }

            ++ShadowCastersCount;
        }
    }

    for (auto it = _gameObject->Children.cbegin(); it != _gameObject->Children.cend(); it++)
        CollectRenderers(*it);
}