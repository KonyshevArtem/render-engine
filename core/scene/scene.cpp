#include "scene.h"
#include "arguments.h"
#include "file_system/file_system.h"
#include "nlohmann/json.hpp"
#include "vector4/vector4.h"
#include "camera/camera.h"

struct ComponentInfo
{
    std::string Name;
    nlohmann::json Data;
};

struct GameObjectInfo
{
    std::string Name;
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scale;
    std::vector<ComponentInfo> Components;
};

void from_json(const nlohmann::json& json, ComponentInfo& info)
{
    json.at("Name").get_to(info.Name);
    json.at("Data").get_to(info.Data);
}

void from_json(const nlohmann::json& json, Vector3& vector)
{
    json.at("x").get_to(vector.x);
    json.at("y").get_to(vector.y);
    json.at("z").get_to(vector.z);
}

void from_json(const nlohmann::json& json, Quaternion& vector)
{
    json.at("x").get_to(vector.x);
    json.at("y").get_to(vector.y);
    json.at("z").get_to(vector.z);
    json.at("w").get_to(vector.w);
}

void from_json(const nlohmann::json& json, GameObjectInfo& info)
{
    json.at("Name").get_to(info.Name);
    json.at("Position").get_to(info.Position);
    json.at("Rotation").get_to(info.Rotation);
    json.at("Scale").get_to(info.Scale);
    json.at("Components").get_to(info.Components);
}

void Scene::Update()
{
    if (Current != nullptr)
    {
        Current->UpdateInternal();
    }
    else
    {
        Load("core_resources/scenes/pbr_demo.scene");
    }
}
void Scene::Load(const std::string& scenePath)
{
    std::string scene = FileSystem::ReadFile(scenePath);
    nlohmann::json sceneJson = nlohmann::json::parse(scene);

    std::vector<GameObjectInfo> gameObjects = std::move(sceneJson.at("GameObjects").template get<std::vector<GameObjectInfo>>());

    Current = std::make_shared<Scene>();

    for (const GameObjectInfo& info : gameObjects)
    {
        std::shared_ptr<GameObject> go = GameObject::Create(info.Name);
        go->SetLocalPosition(info.Position);
        go->SetLocalRotation(info.Rotation);
        go->SetLocalScale(info.Scale);

        for (const ComponentInfo& componentInfo: info.Components)
        {
            go->Components.push_back(Component::CreateComponent(componentInfo.Name, componentInfo.Data));
        }
    }
}
