#include "scene.h"
#include "arguments.h"
#include "scene_parser.h"
#include "component/component.h"

namespace SceneLocal
{
    std::shared_ptr<GameObject> FindGameObject(const std::vector<std::shared_ptr<GameObject>>& gameObjects, const std::function<bool(const GameObject*)>& predicate)
    {
        for (const std::shared_ptr<GameObject>& go : gameObjects)
        {
            if (predicate(go.get()))
                return go;

            std::shared_ptr<GameObject> result = FindGameObject(go->Children, predicate);
            if (result)
                return result;
        }

        return nullptr;
    }

    void FindGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects, const std::function<bool(const GameObject*)>& predicate, std::vector<std::shared_ptr<GameObject>>& result)
    {
        for (const std::shared_ptr<GameObject>& go : gameObjects)
        {
            if (predicate(go.get()))
                result.push_back(go);

            FindGameObjects(go->Children, predicate, result);
        }
    }
}

void Scene::Update()
{
    if (Current != nullptr)
    {
        for (const std::shared_ptr<GameObject>& go : Current->m_GameObjects)
        {
            for (const std::shared_ptr<Component>& component: go->m_Components)
                component->Update();
        }

        Current->UpdateInternal();
    }
    else
    {
        Load("core_resources/scenes/pbr_demo.scene");
    }
}
void Scene::Load(const std::string& scenePath)
{
    Current = SceneParser::Parse(scenePath);
}

std::shared_ptr<GameObject> Scene::FindGameObject(const std::function<bool(const GameObject *)>& predicate)
{
    return SceneLocal::FindGameObject(m_GameObjects, predicate);
}

std::vector<std::shared_ptr<GameObject>> Scene::FindGameObjects(const std::function<bool(const GameObject*)>& predicate)
{
    std::vector<std::shared_ptr<GameObject>> result;
    SceneLocal::FindGameObjects(m_GameObjects, predicate, result);
    return result;
}
