#ifndef RENDER_ENGINE_CONTEXT_H
#define RENDER_ENGINE_CONTEXT_H

#include "matrix4x4/matrix4x4.h"
#include "bounds/bounds.h"

#include <vector>
#include <memory>

class Renderer;
class Light;
class Texture;
class Cubemap;
class GameObject;

struct Context
{
public:
    Context();

    std::vector<Light *> Lights;

    std::vector<std::shared_ptr<Renderer>> Renderers;

    Matrix4x4 ViewMatrix = Matrix4x4();
    Matrix4x4 ProjectionMatrix = Matrix4x4();

    std::shared_ptr<Cubemap> Skybox;

    Bounds ShadowCasterBounds;
    int ShadowCastersCount = 0;
    float ShadowDistance = 0;

private:
    void CollectRenderers(const std::shared_ptr<GameObject> &_gameObject);
};

#endif //RENDER_ENGINE_CONTEXT_H
