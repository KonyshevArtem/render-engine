#include "test_scene_updater.h"
#include "material/material.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "time/time.h"
#include "math_utils.h"
#include "renderer/mesh_renderer.h"
#include "fbx_asset/fbx_asset.h"

std::shared_ptr<TestSceneUpdater> TestSceneUpdater::Create(const nlohmann::json& componentData)
{
    return std::make_shared<TestSceneUpdater>();
}

namespace TestSceneUpdaterLocal
{
    constexpr float k_LoopDuration = 3;

    Vector3 CalcTranslation(float phase)
    {
        const float radius = 2;

        float xOffset = sinf(phase * 2 * static_cast<float>(M_PI)) * radius;
        float yOffset = cosf(phase * 2 * static_cast<float>(M_PI)) * radius;

        return {xOffset, yOffset, 5};
    }

    Quaternion CalcRotation(float phase, int i)
    {
        Vector3 axis = Vector3(i == 0 ? 1 : 0, i == 0 ? 0 : 1, 0);
        return Quaternion::AngleAxis(360 * phase, axis);
    }

    Vector3 CalcScale(float phase)
    {
        float scale = Math::Lerp(1, 2, (sinf(phase * 2 * static_cast<float>(M_PI)) + 1) * 0.5f);
        return {scale, scale, scale};
    }
}

void TestSceneUpdater::Update()
{
    if (!m_Initialized)
    {
        m_DirLight = Scene::Current->FindGameObject([](const GameObject* go){return go->Name=="Directional Light";});
        m_RotatingCube = Scene::Current->FindGameObject([](const GameObject* go){return go->Name=="Rotating Cube";});
        m_RotatingCylinder1 = Scene::Current->FindGameObject([](const GameObject* go){return go->Name=="Rotating Cylinder 1";});
        m_RotatingCylinder2 = Scene::Current->FindGameObject([](const GameObject* go){return go->Name=="Rotating Cylinder 2";});
        m_WaterMaterial = Scene::Current->FindGameObject([](const GameObject* go){return go->Name == "Water";})->GetRenderer()->GetMaterial();


        std::shared_ptr<Mesh> cubeMesh = FBXAsset::Load("core_resources/models/cube.fbx")->GetMesh(0);
        std::shared_ptr<Material> sphereMaterial = Material::Load("core_resources/materials/test_scene/sphere_instanced.material");

        constexpr int spheresCount = 500;
        constexpr int gridSize = 20;
        for (int i = 0; i < spheresCount; ++i)
        {
            std::shared_ptr<GameObject> instancedCube = GameObject::Create("Instanced Cube " + std::to_string(i));
            float x = (i % (gridSize * gridSize)) % gridSize;
            int y = i / (gridSize * gridSize);
            float z = (i % (gridSize * gridSize)) / gridSize;
            instancedCube->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, sphereMaterial));
            instancedCube->SetLocalPosition({-40.0f + 80.0f * x / gridSize, -15.0f - 2.5f * y, -40.0f + 80.0f * z / gridSize});
            instancedCube->GetRenderer()->CastShadows = false;
        }

        m_Initialized = true;
    }

    float phase = fmodf(fmodf(Time::GetElapsedTime(), TestSceneUpdaterLocal::k_LoopDuration) / TestSceneUpdaterLocal::k_LoopDuration, 1.0f);

    if (!m_DirLight.expired())
    {
        GameObject* light = m_DirLight.lock().get();
        light->SetRotation(Quaternion::AngleAxis(50.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * light->GetRotation());
    }

    if (!m_WaterMaterial.expired())
    {
        Material* mat = m_WaterMaterial.lock().get();
        float offset = Math::Lerp(0, 1, phase);
        mat->SetVector("_Albedo_ST", Vector4(offset, offset, 3, 3));
        mat->SetVector("_NormalMap_ST", Vector4(offset, offset, 3, 3));
    }

    if (!m_RotatingCube.expired())
    {
        auto cube = m_RotatingCube.lock();
        cube->SetPosition(TestSceneUpdaterLocal::CalcTranslation(phase));
        cube->SetLocalRotation(TestSceneUpdaterLocal::CalcRotation(phase, 0));
        cube->SetLocalScale(TestSceneUpdaterLocal::CalcScale(phase));
    }

    if (!m_RotatingCylinder1.expired())
        m_RotatingCylinder1.lock()->SetLocalRotation(TestSceneUpdaterLocal::CalcRotation(phase, 1));

    if (!m_RotatingCylinder2.expired())
        m_RotatingCylinder2.lock()->SetLocalRotation(TestSceneUpdaterLocal::CalcRotation(phase, 1));
}