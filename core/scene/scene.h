#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include "gameObject/gameObject.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <vector>

class Light;
class Cubemap;

class Scene
{
public:
    inline static std::shared_ptr<Scene> Current = nullptr;
    std::vector<std::shared_ptr<Light>>  Lights;
    Vector3                              AmbientLight;
    std::shared_ptr<Cubemap>             Skybox;

    static void Update();

    inline std::vector<std::shared_ptr<GameObject>> &GetRootGameObjects()
    {
        return m_GameObjects;
    }

private:
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;

    virtual void UpdateInternal() = 0;
};

#endif //RENDER_ENGINE_SCENE_H
