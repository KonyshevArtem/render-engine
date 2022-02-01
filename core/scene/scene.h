#ifndef OPENGL_STUDY_SCENE_H
#define OPENGL_STUDY_SCENE_H

#include "../../math/vector3/vector3.h"
#include <memory>
#include <vector>

class Light;
class GameObject;
class Cubemap;

class Scene
{
public:
    inline static std::shared_ptr<Scene>     Current = nullptr;
    std::vector<std::shared_ptr<GameObject>> GameObjects;
    std::vector<std::shared_ptr<Light>>      Lights;
    Vector3                                  AmbientLight;
    std::shared_ptr<Cubemap>                 Skybox;

    static void Update();

private:
    virtual void UpdateInternal() = 0;
};

#endif //OPENGL_STUDY_SCENE_H
