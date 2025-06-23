#include "shadows_demo.h"
#include "camera/camera.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "shader/shader.h"
#include "material/material.h"
#include "renderer/mesh_renderer.h"
#include "light/light.h"
#include "input/input.h"
#include "texture_2d/texture_2d.h"
#include <memory>

void ShadowsDemo::Load()
{
    auto scene = std::make_shared<ShadowsDemo>();
    Scene::Current = scene;
    scene->Init();
}

void ShadowsDemo::Init()
{
    //init camera
    Camera::Init(60, 0.5f, 100, 100);

    // init meshes
    auto cubeMesh = FBXAsset::Load("core_resources/models/cube.fbx")->GetMesh(0);
    auto sphereMesh = FBXAsset::Load("core_resources/models/sphere.fbx")->GetMesh(0);

    // init materials
    auto standardOpaqueMaterial = Material::Load("core_resources/materials/shadows_demo/shadows_demo.material");

    // init walls
    {
        auto wall = GameObject::Create("");
        wall->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
        wall->SetLocalPosition({0, 0, 15});
        wall->SetLocalScale({5, 5, 1});

        wall = GameObject::Create("");
        wall->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
        wall->SetLocalPosition({-5, 0, 10});
        wall->SetLocalScale({1, 5, 5});

        wall = GameObject::Create("");
        wall->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
        wall->SetLocalPosition({5, 0, 10});
        wall->SetLocalScale({1, 5, 5});

        wall = GameObject::Create("");
        wall->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
        wall->SetLocalPosition({0, 5, 10});
        wall->SetLocalScale({5, 1, 5});

        wall = GameObject::Create("");
        wall->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
        wall->SetLocalPosition({0, -5, 10});
        wall->SetLocalScale({5, 1, 5});
    }

    // init spheres
    {
        auto sphere = GameObject::Create("");
        sphere->AddComponent(std::make_shared<MeshRenderer>(sphereMesh, standardOpaqueMaterial));
        sphere->SetLocalPosition({-3, 1, 13});
        sphere->SetLocalScale({0.5f, 0.5f, 0.5f});

        sphere = GameObject::Create("");
        sphere->AddComponent(std::make_shared<MeshRenderer>(sphereMesh, standardOpaqueMaterial));
        sphere->SetLocalPosition({2, -3, 9});
        sphere->SetLocalScale({0.5f, 0.5f, 0.5f});

        sphere = GameObject::Create("");
        sphere->AddComponent(std::make_shared<MeshRenderer>(sphereMesh, standardOpaqueMaterial));
        sphere->SetLocalPosition({1, 3, 13 });
        sphere->SetLocalScale({0.5f, 0.5f, 0.5f});
    }

    auto pointLight1 = std::make_shared<Light>();
    pointLight1->Position = Vector3(2, 2, 12);
    pointLight1->Intensity = Vector3(1, 1, 1);
    pointLight1->Range = 30;
    pointLight1->Type = LightType::POINT;

    auto pointLight2 = std::make_shared<Light>();
    pointLight2->Position = Vector3(-2, -2, 8);
    pointLight2->Intensity = Vector3(1, 1, 1);
    pointLight2->Range = 30;
    pointLight2->Type = LightType::POINT;

    Lights.push_back(pointLight1);
    Lights.push_back(pointLight2);

    m_CameraFlyControl = std::make_unique<CameraFlyController>();
}

void ShadowsDemo::UpdateInternal()
{
    m_CameraFlyControl->Update();
}
