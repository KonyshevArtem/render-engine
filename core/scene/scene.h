#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include "gameObject/gameObject.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

class Light;
class Cubemap;

class Scene
{
public:
    inline static std::shared_ptr<Scene> Current = nullptr;
    std::shared_ptr<Cubemap>             Skybox;

    static void Update();
    static void Load(const std::string& scenePath);

    std::shared_ptr<GameObject> FindGameObject(const std::function<bool(const GameObject*)>& predicate);
    std::vector<std::shared_ptr<GameObject>> FindGameObjects(const std::function<bool(const GameObject*)>& predicate);

    inline std::vector<std::shared_ptr<GameObject>> &GetRootGameObjects()
    {
        return m_GameObjects;
    }

private:
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;

    static void UpdateComponents(std::vector<std::shared_ptr<GameObject>>& gameObjects);
};

#endif //RENDER_ENGINE_SCENE_H
