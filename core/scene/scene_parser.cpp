#include "scene_parser.h"
#include "file_system/file_system.h"
#include "vector3/vector3.h"
#include "quaternion/quaternion.h"
#include "component/component.h"
#include "nlohmann/json.hpp"

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

namespace SceneParser
{
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

    struct SceneInfo
    {
        std::vector<GameObjectInfo> GameObjects;
    };

    void from_json(const nlohmann::json& json, ComponentInfo& info)
    {
        json.at("Name").get_to(info.Name);
        json.at("Data").get_to(info.Data);
    }

    void from_json(const nlohmann::json& json, GameObjectInfo& info)
    {
        json.at("Name").get_to(info.Name);
        json.at("Position").get_to(info.Position);
        json.at("Rotation").get_to(info.Rotation);
        json.at("Scale").get_to(info.Scale);
        if (json.contains("Components"))
            json.at("Components").get_to(info.Components);
    }

    void from_json(const nlohmann::json& json, SceneInfo& info)
    {
        json.at("GameObjects").get_to(info.GameObjects);
    }

    std::shared_ptr<Scene> Parse(const std::string& path)
    {
        std::string sceneText = FileSystem::ReadFile(path);
        nlohmann::json sceneJson = nlohmann::json::parse(sceneText);

        SceneInfo sceneInfo;
        sceneJson.get_to(sceneInfo);

        std::shared_ptr<Scene> scene = std::make_shared<Scene>();

        for (const GameObjectInfo& info : sceneInfo.GameObjects)
        {
            std::shared_ptr<GameObject> go = GameObject::Create(info.Name, scene);

            go->SetLocalPosition(info.Position);
            go->SetLocalRotation(info.Rotation);
            go->SetLocalScale(info.Scale);

            for (const ComponentInfo& componentInfo: info.Components)
            {
                std::shared_ptr<Component> component = Component::CreateComponent(componentInfo.Name, componentInfo.Data);
                go->AddComponent(component);
            }
        }

        return scene;
    }
}
