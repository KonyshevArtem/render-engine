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
    Context();

    std::vector<std::shared_ptr<Light>> Lights;

    std::vector<std::shared_ptr<Renderer>> Renderers;

    Matrix4x4 ViewMatrix = Matrix4x4();
    Matrix4x4 ProjectionMatrix = Matrix4x4();

    std::shared_ptr<Cubemap> Skybox;

private:
    void CollectRenderers(const std::shared_ptr<GameObject> &gameObject);
};

#endif //RENDER_ENGINE_CONTEXT_H
