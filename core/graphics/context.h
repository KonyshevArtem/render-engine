#ifndef OPENGL_STUDY_CONTEXT_H
#define OPENGL_STUDY_CONTEXT_H

#include "matrix4x4/matrix4x4.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Renderer;
class Light;
class Texture;
class Cubemap;
class GameObject;

struct Context
{
public:
    Context();

    Vector3                  AmbientLight;
    std::vector<Light *>     Lights;
    std::vector<Renderer *>  Renderers;
    Matrix4x4                ViewMatrix       = Matrix4x4();
    Matrix4x4                ProjectionMatrix = Matrix4x4();
    std::shared_ptr<Cubemap> Skybox;
    float                    ShadowDistance;

private:
    void CollectRenderers(const std::shared_ptr<GameObject> &_gameObject);
};

#endif //OPENGL_STUDY_CONTEXT_H
