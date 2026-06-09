#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include "gameObject/gameObject.h"
#include "texture/texture_resources.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <shared_mutex>

class Light;

class Scene
{
public:
    inline static std::shared_ptr<Scene> Current = nullptr;

    Scene();

    static void Init();
    static void Update();
    static void Load(const std::string& scenePath);
    static void Unload();

    void SetSkybox(const std::shared_ptr<Texture>& skybox);
    std::shared_ptr<TextureView> GetSkybox();

    std::shared_ptr<GameObject> FindGameObject(const std::function<bool(const GameObject*)>& predicate);
    std::vector<std::shared_ptr<GameObject>> FindGameObjects(const std::function<bool(const GameObject*)>& predicate);

    inline std::vector<std::shared_ptr<GameObject>> &GetRootGameObjects()
    {
        return m_GameObjects;
    }

    bool IsLoading();
    void SetLoading(bool isLoading);

private:
    static std::filesystem::path s_PendingScenePath;

    std::vector<std::shared_ptr<GameObject>> m_GameObjects;

    std::shared_mutex m_SkyboxMutex;
    TextureResources m_Skybox;

    std::atomic<bool> m_IsLoading;

    static void LoadInternal();
    static void UpdateComponents(std::vector<std::shared_ptr<GameObject>>& gameObjects);
};

#endif //RENDER_ENGINE_SCENE_H
