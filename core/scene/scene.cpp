#include "scene.h"
#include "arguments.h"
#include "scene_parser.h"
#include "component/component.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "ui/ui_manager.h"

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

    template<typename T, typename Func>
    void UpdateCollection(std::vector<std::shared_ptr<T>>& collection, const Func& updateFunction)
    {
        for (int i = 0; i < collection.size(); ++i)
        {
            int originalSize = collection.size();
                updateFunction(collection[i].get());

            if (originalSize != collection.size())
                i = -1;
        }
    }
}

std::filesystem::path Scene::s_PendingScenePath = "";

void Scene::Update()
{
    if (!s_PendingScenePath.empty())
        LoadInternal();

    if (Current != nullptr && !Current->IsLoading())
    {
        Profiler::Marker _("Scene::UpdateComponents");
        UpdateComponents(Current->m_GameObjects);
    }
}

void Scene::Load(const std::string& scenePath)
{
    s_PendingScenePath = scenePath;
}

void Scene::Unload()
{
    Current = nullptr;
    UIManager::DestroyUI();
}

void Scene::SetSkybox(const std::shared_ptr<Cubemap>& skybox)
{
    std::unique_lock lock(m_SkyboxMutex);
    m_Skybox = skybox;
}

std::shared_ptr<Cubemap> Scene::GetSkybox()
{
    std::shared_lock lock(m_SkyboxMutex);
    return m_Skybox;
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

bool Scene::IsLoading()
{
    return m_IsLoading;
}

void Scene::SetLoading(bool isLoading)
{
    m_IsLoading = isLoading;
}

void Scene::LoadInternal()
{
    Profiler::Marker _("Scene::LoadInternal");

    Unload();
    Current = SceneParser::Parse(s_PendingScenePath);
    s_PendingScenePath = "";
}

void Scene::UpdateComponents(std::vector<std::shared_ptr<GameObject>> &gameObjects)
{
    uint64_t frame = GraphicsBackend::Current()->GetFrameNumber();

    auto updateComponent = [frame](Component* component)
    {
        if (component->m_LastUpdatedFrame < frame)
        {
            component->Update();
            component->m_LastUpdatedFrame = frame;
        }
    };

    SceneLocal::UpdateCollection(gameObjects, [&updateComponent](GameObject* go)
    {
        SceneLocal::UpdateCollection(go->m_Components, updateComponent);

        UpdateComponents(go->Children);
    });
}
