#ifndef RENDER_ENGINE_CONTEXT_H
#define RENDER_ENGINE_CONTEXT_H

#include "matrix4x4/matrix4x4.h"
#include "vector2/vector2.h"
#include "texture/texture_resources.h"

#include <vector>
#include <memory>

class Renderer;
class Light;
class Cubemap;
class GameObject;

struct RenderData
{
    static RenderData GetRenderData(int viewportWidth, int viewportHeight);

    std::vector<Light*> Lights;

    std::vector<std::shared_ptr<Renderer>> Renderers;

    Vector2 Viewport;
    float FoV;
    float NearPlane;
    float FarPlane;

    Matrix4x4 ViewMatrix = Matrix4x4();
    Matrix4x4 ProjectionMatrix = Matrix4x4();

    std::shared_ptr<TextureView> Skybox;

	TextureResources GBuffers[2];
    TextureResources CameraColorTarget;
    TextureResources CameraDepthTarget;

    TextureResources RaytracedShadowsTarget;
    bool RaytracedShadowsEnabled;

    TextureResources PostProcessedTarget;

private:
    void CollectRenderers(const std::shared_ptr<GameObject> &gameObject);
};

#endif //RENDER_ENGINE_CONTEXT_H
