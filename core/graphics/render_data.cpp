#include "render_data.h"
#include "camera/camera.h"
#include "gameObject/gameObject.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "light/light.h"

RenderData RenderData::GetRenderData(int viewportWidth, int viewportHeight)
{
    RenderData data{};

    if (Scene::Current == nullptr || Camera::Current == nullptr)
        return data;

    const std::shared_ptr<Scene>& scene = Scene::Current;
    const std::shared_ptr<Camera> camera = Camera::Current;

    data.Viewport = Vector2(viewportWidth, viewportHeight);
    data.FoV = camera->GetFov();
    data.NearPlane = camera->GetNearClipPlane();
    data.FarPlane = camera->GetFarClipPlane();

    data.Skybox = scene->GetSkybox();
    data.ViewMatrix = camera->GetGameObject()->GetWorldToLocalMatrix();
    data.ProjectionMatrix = Matrix4x4::Perspective(data.FoV, data.Viewport.x / data.Viewport.y, camera->GetNearClipPlane(), camera->GetFarClipPlane());

    const std::vector<std::shared_ptr<GameObject>>& gameObjects = Scene::Current->GetRootGameObjects();
    for (const std::shared_ptr<GameObject>& go : gameObjects)
        data.CollectRenderers(go);

    for (Light* light : Light::s_Lights)
    {
        if (light != nullptr)
            data.Lights.push_back(light);
    }

    return data;
}

void RenderData::CollectRenderers(const std::shared_ptr<GameObject> &gameObject)
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
