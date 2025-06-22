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

std::unordered_map<std::string, Component::Factory*> &Component::GetFactories()
{
    static std::unordered_map<std::string, Factory*> factories;
    return factories;
}
