#include "pbr_demo_updater.h"
#include "scene/scene.h"
#include "renderer/renderer.h"
#include "material/material.h"
#include "time/time.h"

std::shared_ptr<PBRDemoUpdater> PBRDemoUpdater::Create(const nlohmann::json& componentData)
{
    return std::make_shared<PBRDemoUpdater>();
}

DEFINE_COMPONENT_DEFAULT_ASYNC_CONSTRUCTOR(PBRDemoUpdater)

void PBRDemoUpdater::Update()
{
    if (!m_Initialized)
    {
        std::vector<std::shared_ptr<GameObject>> spheres = Scene::Current->FindGameObjects([](const GameObject* go) {return go->GetRenderer() != nullptr;});
        for (int i = 0; i < spheres.size(); ++i)
        {
            std::shared_ptr<GameObject>& go = spheres[i];
            std::shared_ptr<Renderer> renderer = go->GetRenderer();
            std::shared_ptr<Material> material = renderer->GetMaterial()->Copy();
            renderer->SetMaterial(material);

            material->SetFloat("_Roughness", 1.0f - static_cast<float>(i % 6) / 5);
            material->SetFloat("_Metallness", static_cast<float>(i / 6) / 5);
        }

        m_DirLight = Scene::Current->FindGameObject([](const GameObject* go){return go->Name=="Directional Light";});

        m_Initialized = true;
    }

    if (!m_DirLight.expired())
    {
        GameObject* light = m_DirLight.lock().get();
        light->SetRotation(Quaternion::AngleAxis(80.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * light->GetRotation());
    }
}