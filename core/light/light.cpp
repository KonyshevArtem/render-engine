#include "light.h"
#include "json_common/json_common.h"

NLOHMANN_JSON_SERIALIZE_ENUM(LightType,
{
     {LightType::DIRECTIONAL, "DIRECTIONAL"},
     {LightType::POINT, "POINT"},
     {LightType::SPOT, "SPOT"}
})

std::shared_ptr<Light> Light::Create(const nlohmann::json& componentData)
{
    std::shared_ptr<Light> light = std::make_shared<Light>();
    componentData.at("Type").get_to(light->Type);
    componentData.at("Intensity").get_to(light->Intensity);
    if (componentData.contains("Range"))
        componentData.at("Range").get_to(light->Range);
    if (componentData.contains("CutOffAngle"))
        componentData.at("CutOffAngle").get_to(light->CutOffAngle);
    return light;
}

DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR(Light)

std::vector<Light*> Light::s_Lights;

Light::Light()
{
    m_Index = s_Lights.size();
    s_Lights.push_back(this);
}

Light::~Light()
{
    s_Lights.back()->m_Index = m_Index;
    std::swap(s_Lights[m_Index], s_Lights.back());
    s_Lights.pop_back();
}
