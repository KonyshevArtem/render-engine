#include "component.h"
#include "nlohmann/json.hpp"

#include <cassert>

void Component::RegisterComponentFactory(const std::string& componentName, Factory* factory)
{
    assert(!GetFactories().contains(componentName));
    GetFactories()[componentName] = factory;
}

std::shared_ptr<Component> Component::CreateComponent(const std::string& componentName, const nlohmann::json& componentData)
{
    return GetFactories().at(componentName)->CreateComponent(componentData);
}

std::shared_ptr<Worker::Task> Component::CreateComponentAsync(const std::string& componentName, const nlohmann::json& componentData, const std::function<void(std::shared_ptr<Component>)>& callback)
{
    return GetFactories().at(componentName)->CreateComponentAsync(componentData, callback);
}

std::unordered_map<std::string, Component::Factory*> &Component::GetFactories()
{
    static std::unordered_map<std::string, Factory*> factories;
    return factories;
}
