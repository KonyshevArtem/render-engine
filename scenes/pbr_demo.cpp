#include "pbr_demo.h"
#include "camera/camera.h"
#include "fbx_asset/fbx_asset.h"
#include "input/input.h"
#include "light/light.h"
#include "material/material.h"
#include "renderer/mesh_renderer.h"
#include "shader/shader.h"
#include "time/time.h"
#include "cubemap/cubemap.h"
#include "texture_2d/texture_2d.h"
#include <memory>

void PBRDemo::Load()
{
    auto scene     = std::make_shared<PBRDemo>();
    Scene::Current = scene;
    scene->Init();
}

std::shared_ptr<Light> light;

void PBRDemo::Init()
{
    //init camera
    Camera::Init(80, 0.5f, 100, 100);

    //init mesh
    auto sphereAsset = FBXAsset::Load("core_resources/models/sphere.fbx");
    auto sphereMesh  = sphereAsset->GetMesh(0);

    //init skybox
    Skybox = Cubemap::Load("core_resources/textures/skybox/skybox");

    std::shared_ptr<Material> baseMaterial = Material::Load("core_resources/materials/pbr_demo/pbr_demo.material");

    for (float i = 0; i < 6; ++i)
    {
        for (float j = 0; j < 6; ++j)
        {
            //init material
            std::shared_ptr<Material> material = baseMaterial->Copy();
            material->SetFloat("_Roughness", 1 - j / 5);
            material->SetFloat("_Metallness", i / 5);

            //init gameObject
            auto sphere  = GameObject::Create("Sphere");
            sphere->AddComponent(std::make_shared<MeshRenderer>(sphereMesh, material));
            sphere->SetLocalPosition({3 * j, 3 * i, 0});
        }
    }

    //init light
    light            = std::make_shared<Light>();
    light->Rotation  = Quaternion::AngleAxis(-50, Vector3(0, 1, 0)) * Quaternion::AngleAxis(30, Vector3(1, 0, 0));
    light->Intensity = Vector3(0.8f, 0.8f, 0.8f);
    light->Type      = LightType::DIRECTIONAL;
    Lights.push_back(light);

    auto pointLight         = std::make_shared<Light>();
    pointLight->Position    = {7.5f, 7.5f, 0};
    pointLight->Intensity   = {0, 1, 0};
    pointLight->Type        = LightType::POINT;
    pointLight->Range       = 15;
    Lights.push_back(pointLight);

    auto spotLight         = std::make_shared<Light>();
    spotLight->Position    = {7.5f, 7.5f, -16};
    spotLight->Intensity   = {0, 0, 1};
    spotLight->Type        = LightType::SPOT;
    spotLight->CutOffAngle = 30;
    spotLight->Range       = 30;
    Lights.push_back(spotLight);

    Camera::Current->SetPosition({7.5f, 7.5f, -15});
}

void PBRDemo::UpdateInternal()
{
    light->Rotation = Quaternion::AngleAxis(80.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * light->Rotation;
}