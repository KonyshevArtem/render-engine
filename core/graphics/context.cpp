#include "context.h"
#include "camera/camera.h"
#include "gameObject/gameObject.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "light/light.h"
#include "graphics/graphics.h"

Context::Context()
{
    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return;

    const std::shared_ptr<Scene>& scene = Scene::Current;
    const std::shared_ptr<Camera> camera = Camera::Current;

    Viewport = Vector2(Graphics::GetScreenWidth(), Graphics::GetScreenHeight());
    FoV = camera->GetFov();
    NearPlane = camera->GetNearClipPlane();
    FarPlane = camera->GetFarClipPlane();

    Skybox = scene->GetSkybox();
    ViewMatrix = camera->GetGameObject()->GetWorldToLocalMatrix();
    ProjectionMatrix = Matrix4x4::Perspective(FoV, Viewport.x / Viewport.y, camera->GetNearClipPlane(), camera->GetFarClipPlane());

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