#include "context.h"
#include "camera/camera.h"
#include "gameObject/gameObject.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "light/light.h"

Context::Context()
{
    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return;

    const std::shared_ptr<Scene>& scene = Scene::Current;

    Skybox = scene->GetSkybox();
    ViewMatrix = Camera::Current->GetViewMatrix();
    ProjectionMatrix = Camera::Current->GetProjectionMatrix();

    const std::vector<std::shared_ptr<GameObject>>& gameObjects = Scene::Current->GetRootGameObjects();
    for (const std::shared_ptr<GameObject>& go : gameObjects)
        CollectRenderers(go);

    for (Light* light : Light::s_Lights)
    {
        if (light != nullptr)
            Lights.push_back(light);
    }
}

void Context::CollectRenderers(const std::shared_ptr<GameObject> &gameObject)
{
    if (!gameObject)
        return;

    const std::shared_ptr<Renderer>& renderer = gameObject->GetRenderer();
    if (renderer)
    {
        Renderers.push_back(renderer);
    }

    for (const std::shared_ptr<GameObject>& child : gameObject->Children)
        CollectRenderers(child);
}