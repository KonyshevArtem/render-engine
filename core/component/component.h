#ifndef RENDER_ENGINE_COMPONENT_H
#define RENDER_ENGINE_COMPONENT_H

#include "nlohmann/json.hpp"
#include "worker/worker.h"

#include <memory>
#include <string>
#include <unordered_map>

class GameObject;

class Component
{
public:
    class Factory
    {
    public:
        virtual std::shared_ptr<Component> CreateComponent(const nlohmann::json& componentData) = 0;
        virtual std::shared_ptr<Worker::Task> CreateComponentAsync(const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Component>)>& callback) = 0;
    };

    static void RegisterComponentFactory(const std::string& componentName, Factory* factory);
    static std::shared_ptr<Component> CreateComponent(const std::string& componentName, const nlohmann::json& componentData);
    static std::shared_ptr<Worker::Task> CreateComponentAsync(const std::string& componentName, const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Component>)>& callback);

    virtual void Update(){};

    std::shared_ptr<GameObject> GetGameObject()
    {
        return !m_GameObject.expired() ? m_GameObject.lock() : nullptr;
    }

protected:
    std::weak_ptr<GameObject> m_GameObject;
    uint64_t m_LastUpdatedFrame;

private:
    static std::unordered_map<std::string, Factory*>& GetFactories();

    friend class GameObject;
    friend class Scene;
};

#ifndef REGISTER_COMPONENT
#define REGISTER_COMPONENT(Name) \
class Name##ComponentFactory : public Component::Factory \
{ \
public: \
    std::shared_ptr<Component> CreateComponent(const nlohmann::json& componentData) override \
    { \
        return Name::Create(componentData); \
    } \
    std::shared_ptr<Worker::Task> CreateComponentAsync(const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Component>)>& callback) override \
    { \
        return Name::CreateAsync(componentData, callback); \
    } \
    static class Name##ComponentFactoryInitializer \
    { \
    public: \
        Name##ComponentFactoryInitializer() \
        { \
            Component::RegisterComponentFactory(#Name, new Name##ComponentFactory()); \
        } \
    } Initializer; \
};                               \
Name##ComponentFactory::Name##ComponentFactoryInitializer Name##ComponentFactory::Initializer;
#endif

#ifndef DECLARE_COMPONENT_CONSTRUCTORS
#define DECLARE_COMPONENT_CONSTRUCTORS(Name) \
    static std::shared_ptr<Name> Create(const nlohmann::json& componentData); \
    static std::shared_ptr<Worker::Task> CreateAsync(const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Name>)>& callback);
#endif

#ifndef DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR
#define DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR(Name) \
std::shared_ptr<Worker::Task> Name::CreateAsync(const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Name>)>& callback) \
{ \
    std::shared_ptr<Name> component = Name::Create(componentData); \
    callback(component); \
    return Worker::Noop(); \
}
#endif

#endif //RENDER_ENGINE_COMPONENT_H
