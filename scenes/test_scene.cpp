#include "test_scene.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"
#include "bounds/bounds.h"
#include "camera/camera.h"
#include "cubemap/cubemap.h"
#include "editor/gizmos/gizmos.h"
#include "fbx_asset/fbx_asset.h"
#include "input/input.h"
#include "light/light.h"
#include "material/material.h"
#include "math_utils.h"
#include "mesh/mesh.h"
#include "renderer/billboard_renderer.h"
#include "renderer/mesh_renderer.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "time/time.h" // NOLINT(modernize-deprecated-headers)
#include <cmath>
#include <memory>
#include <random>

void TestScene::Load()
{
    auto scene     = std::make_shared<TestScene>();
    Scene::Current = scene;
    scene->Init();
}

void TestScene::Init()
{
    // init camera
    Camera::Init(75, 0.5f, 100, 100);

    // init textures
    auto billboardTree = Texture2D::Load("core_resources/textures/billboard_tree");

    // init skybox cubemap
    Skybox = Cubemap::Load("core_resources/textures/skybox/skybox");

    // init meshes
    auto cubeAsset     = FBXAsset::Load("core_resources/models/cube.fbx");
    auto cylinderAsset = FBXAsset::Load("core_resources/models/cylinder.fbx");
    auto planeAsset    = FBXAsset::Load("core_resources/models/plane.fbx");
    auto carAsset      = FBXAsset::Load("core_resources/models/car.fbx");

    auto cubeMesh     = cubeAsset->GetMesh(0);
    auto cylinderMesh = cylinderAsset->GetMesh(0);
    auto planeMesh    = planeAsset->GetMesh(0);
    auto carMesh      = carAsset->GetMesh(0);

    // init materials
    auto standardOpaqueMaterial = Material::Load("core_resources/materials/test_scene/standard_opaque.material");
    auto brickMaterial = Material::Load("core_resources/materials/test_scene/brick.material");
    m_WaterMaterial = Material::Load("core_resources/materials/test_scene/water.material");
    auto transparentMaterial = Material::Load("core_resources/materials/test_scene/standard_transparent.material");
    auto carMaterial = Material::Load("core_resources/materials/test_scene/car.material");
    auto sphereMaterial = Material::Load("core_resources/materials/test_scene/sphere_instanced.material");

    // init gameObjects
    auto rotatingCube      = GameObject::Create("Rotating Cube");
    rotatingCube->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, brickMaterial));
    m_RotatingCube         = rotatingCube;

    auto rotatingCylinder      = GameObject::Create("Rotating Cylinder");
    rotatingCylinder->AddComponent(std::make_shared<MeshRenderer>(cylinderMesh, standardOpaqueMaterial));
    rotatingCylinder->SetLocalPosition(Vector3(0, -3, 4));
    rotatingCylinder->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder1 = rotatingCylinder;

    auto cylinderFragmentLit      = GameObject::Create("Cylinder");
    cylinderFragmentLit->AddComponent(std::make_shared<MeshRenderer>(cylinderMesh, standardOpaqueMaterial));
    cylinderFragmentLit->SetLocalPosition(Vector3(-3, -3, 6));
    cylinderFragmentLit->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder2 = cylinderFragmentLit;

    auto floorVertexLit      = GameObject::Create("Floor");
    floorVertexLit->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, standardOpaqueMaterial));
    floorVertexLit->SetLocalPosition(Vector3(3, -5, 5.5f));
    floorVertexLit->SetLocalRotation(Quaternion::AngleAxis(-10, Vector3(0, 1, 0)));
    floorVertexLit->SetLocalScale(Vector3(5, 1, 2));

    auto floorFragmentLit      = GameObject::Create("Bricks");
    floorFragmentLit->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, brickMaterial));
    floorFragmentLit->SetLocalPosition(Vector3(-9, -5, 5.5f));
    floorFragmentLit->SetLocalRotation(Quaternion::AngleAxis(10, Vector3(0, 1, 0)));
    floorFragmentLit->SetLocalScale(Vector3(5, 1, 2));

    auto water      = GameObject::Create("Water");
    water->AddComponent(std::make_shared<MeshRenderer>(planeMesh, m_WaterMaterial));
    water->SetLocalPosition(Vector3(0, -10, 10));
    water->SetLocalScale(Vector3(20, 1, 20));

    auto car      = GameObject::Create("Car");
    car->AddComponent(std::make_shared<MeshRenderer>(carMesh, carMaterial));
    car->SetLocalPosition(Vector3 {11, -8.5f, -2});
    car->SetLocalRotation(Quaternion::AngleAxis(135, Vector3 {0, -1, 0}));
    car->SetLocalScale(Vector3 {0.02f, 0.02f, 0.02f});

    for (int i = 0; i < 4; ++i)
    {
        std::string name = "TreeBillboard_" + std::to_string(i);
        auto billboard = GameObject::Create(name);
        auto billboardRenderer = std::make_shared<BillboardRenderer>(billboardTree, name);
        billboardRenderer->SetSize(5);
        billboard->SetLocalPosition(Vector3 {-20.0f + 10 * i, -10, 20});
        billboard->AddComponent(billboardRenderer);
    }

    for (int i = 0; i < 3; ++i)
    {
        auto transparentCube      = GameObject::Create("Transparent Cube " + std::to_string(i));
        transparentCube->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, transparentMaterial));
        transparentCube->SetLocalPosition(Vector3(-10.0f + 5 * i, -5, 12));
        transparentCube->SetLocalRotation(Quaternion::AngleAxis(30, {0, 1, 0}));
    }

    auto spheresParent = GameObject::Create("Spheres Parent");

    std::default_random_engine random;
    std::uniform_real_distribution<float> colorDistribution(0, 1);
    std::uniform_real_distribution<float> sizeDistribution(0.75f, 1.25f);
    constexpr int spheresCount = 500;
    constexpr int gridSize     = 20;
    for (int i = 0; i < spheresCount; ++i)
    {
        auto instancedCube = GameObject::Create("Instanced Cube " + std::to_string(i));
        float x = (i % (gridSize * gridSize)) % gridSize;
        int y = i / (gridSize * gridSize);
        float z = (i % (gridSize * gridSize)) / gridSize;
        instancedCube->AddComponent(std::make_shared<MeshRenderer>(cubeMesh, sphereMaterial));
        instancedCube->SetLocalPosition({-40.0f + 80.0f * x / gridSize, -15.0f - 2.5f * y, -40.0f + 80.0f * z / gridSize});
        instancedCube->GetRenderer()->CastShadows = false;

        instancedCube->SetParent(spheresParent);
    }

    // init lights
    m_DirectionalLight            = std::make_shared<Light>();
    m_DirectionalLight->Position  = Vector3(0, -0.3f, -1);
    m_DirectionalLight->Rotation  = Quaternion::AngleAxis(-150, Vector3(0, 1, 0)) * Quaternion::AngleAxis(30, Vector3(1, 0, 0));
    m_DirectionalLight->Type      = LightType::DIRECTIONAL;

    auto pointLight         = std::make_shared<Light>();
    pointLight->Position    = Vector3(-3, -3, 4);
    pointLight->Intensity   = Vector3(10, 0, 0);
    pointLight->Range       = 10;
    pointLight->Type        = LightType::POINT;

    m_SpotLight              = std::make_shared<Light>();
    m_SpotLight->Intensity   = Vector3(10, 10, 10);
    m_SpotLight->Range       = 40;
    m_SpotLight->CutOffAngle = 15;
    m_SpotLight->Type        = LightType::SPOT;

    auto spotLight2         = std::make_shared<Light>();
    spotLight2->Position    = Vector3(-9, 5, 5.5f);
    spotLight2->Rotation    = Quaternion::AngleAxis(90, Vector3(1, 0, 0));
    spotLight2->Intensity   = Vector3(1, 1, 1);
    spotLight2->Range       = 15;
    spotLight2->CutOffAngle = 15;
    spotLight2->Type        = LightType::SPOT;

    Lights.push_back(m_DirectionalLight);
    Lights.push_back(pointLight);
    Lights.push_back(m_SpotLight);
    Lights.push_back(spotLight2);

    // init camera
    Camera::Current->SetPosition(Vector3(-10, 0.5f, -5));
    m_CameraFlyControl = std::make_unique<CameraFlyController>();
}

Vector3 TestScene::CalcTranslation(float _phase)
{
    const float radius = 2;

    float xOffset = sinf(_phase * 2 * static_cast<float>(M_PI)) * radius;
    float yOffset = cosf(_phase * 2 * static_cast<float>(M_PI)) * radius;

    return {xOffset, yOffset, 5};
}

Quaternion TestScene::CalcRotation(float _phase, int _i)
{
    Vector3 axis = Vector3(_i == 0 ? 1 : 0, _i == 0 ? 0 : 1, 0);
    return Quaternion::AngleAxis(360 * _phase, axis);
}

Vector3 TestScene::CalcScale(float _phase)
{
    float scale = Math::Lerp(1, 2, (sinf(_phase * 2 * static_cast<float>(M_PI)) + 1) * 0.5f);
    return {scale, scale, scale};
}

void TestScene::UpdateInternal()
{
    m_CameraFlyControl->Update();

    float phase = fmodf(fmodf(Time::GetElapsedTime(), LOOP_DURATION) / LOOP_DURATION, 1.0f);

    if (!m_RotatingCube.expired())
    {
        auto cube = m_RotatingCube.lock();
        cube->SetPosition(CalcTranslation(phase));
        cube->SetLocalRotation(CalcRotation(phase, 0));
        cube->SetLocalScale(CalcScale(phase));
    }

    if (!m_RotatingCylinder1.expired())
        m_RotatingCylinder1.lock()->SetLocalRotation(CalcRotation(phase, 1));

    if (!m_RotatingCylinder2.expired())
        m_RotatingCylinder2.lock()->SetLocalRotation(CalcRotation(phase, 1));

    // animateWater
    float offset = Math::Lerp(0, 1, phase);
    m_WaterMaterial->SetVector("_Albedo_ST", Vector4(offset, offset, 3, 3));
    m_WaterMaterial->SetVector("_NormalMap_ST", Vector4(offset, offset, 3, 3));

    // animate light
    m_DirectionalLight->Rotation = Quaternion::AngleAxis(50.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * m_DirectionalLight->Rotation;
    m_SpotLight->Position        = Camera::Current->GetPosition() + Camera::Current->GetRotation() * Vector3(-3, 0, 0);
    m_SpotLight->Rotation        = Camera::Current->GetRotation();
}