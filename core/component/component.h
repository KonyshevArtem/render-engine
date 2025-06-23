#ifndef RENDER_ENGINE_COMPONENT_H
#define RENDER_ENGINE_COMPONENT_H

#include "nlohmann/json.hpp"

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
    };

    static void RegisterComponentFactory(const std::string& componentName, Factory* factory);
    static std::shared_ptr<Component> CreateComponent(const std::string& componentName, const nlohmann::json& componentData);

protected:
    std::weak_ptr<GameObject> m_GameObject;

    virtual void Stub(){}; // needed for a class to be polymorphic

private:
    static std::unordered_map<std::string, Factory*>& GetFactories();

    friend class GameObject;
};

#ifndef REGISTER_COMPONENT
#define REGISTER_COMPONENT(Name) \
class Name##ComponentFactory : public Component::Factory \
{ \
public: \
    std::shared_ptr<Component> CreateComponent(const nlohmann::json& componentData) override; \
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

#endif //RENDER_ENGINE_COMPONENT_H
