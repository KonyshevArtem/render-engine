#include "test_scene.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"
#include "bounds/bounds.h"
#include "camera/camera.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "gizmos/gizmos.h"
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
#include "vector4/vector4.h"
#include <cmath>
#include <memory>

void TestScene::Load()
{
    auto scene     = std::make_shared<TestScene>();
    Scene::Current = scene;
    scene->Init();
}

void TestScene::Init()
{
    // init camera
    Camera::Init(120, 0.5f, 100, 100);

    // init textures
    auto brickTexture  = Texture2D::Load("resources/textures/brick.png");
    auto brickNormal   = Texture2D::Load("resources/textures/brick_normal.png", false);
    auto waterTexture  = Texture2D::Load("resources/textures/water.png");
    auto waterNormal   = Texture2D::Load("resources/textures/water_normal.png", false);
    auto billboardTree = Texture2D::Load("resources/textures/billboard_tree.png", true, true);
    auto windowTexture = Texture2D::Load("resources/textures/window_cube.png", true, true);
    auto carAlbedo     = Texture2D::Load("resources/textures/car/car_albedo.png");
    auto carNormal     = Texture2D::Load("resources/textures/car/car_normal.png", false);

    // init skybox cubemap
    Skybox = Cubemap::Load("resources/textures/skybox/x_positive.png",
                           "resources/textures/skybox/x_negative.png",
                           "resources/textures/skybox/y_positive.png",
                           "resources/textures/skybox/y_negative.png",
                           "resources/textures/skybox/z_positive.png",
                           "resources/textures/skybox/z_negative.png");

    // init shaders
    auto fragmentLitShader = Shader::Load("resources/shaders/standard/standard.shader", {"_SMOOTHNESS", "_RECEIVE_SHADOWS", "_NORMAL_MAP"});
    auto transparentShader = Shader::Load("resources/shaders/standard/standard_transparent.shader", {"_SMOOTHNESS", "_RECEIVE_SHADOWS"});

    // init meshes
    auto cubeAsset     = FBXAsset::Load("resources/models/cube.fbx");
    auto cylinderAsset = FBXAsset::Load("resources/models/cylinder.fbx");
    auto planeAsset    = FBXAsset::Load("resources/models/plane.fbx");
    auto carAsset      = FBXAsset::Load("resources/models/car.fbx");

    auto cubeMesh     = cubeAsset->GetMesh(0);
    auto cylinderMesh = cylinderAsset->GetMesh(0);
    auto planeMesh    = planeAsset->GetMesh(0);
    auto carMesh      = carAsset->GetMesh(0);

    // init materials
    auto fragmentLitMaterial = std::make_shared<Material>(fragmentLitShader);
    fragmentLitMaterial->SetFloat("_Smoothness", 50);

    auto fragmentLitBrickMaterial = std::make_shared<Material>(fragmentLitShader);
    fragmentLitBrickMaterial->SetTexture("_Albedo", brickTexture);
    fragmentLitBrickMaterial->SetTexture("_NormalMap", brickNormal);
    fragmentLitBrickMaterial->SetFloat("_Smoothness", 10);

    m_WaterMaterial = std::make_shared<Material>(fragmentLitShader);
    m_WaterMaterial->SetTexture("_Albedo", waterTexture);
    m_WaterMaterial->SetTexture("_NormalMap", waterNormal);
    m_WaterMaterial->SetFloat("_Smoothness", 20);

    auto transparentMaterial = std::make_shared<Material>(transparentShader);
    transparentMaterial->SetTexture("_Albedo", windowTexture);
    transparentMaterial->SetRenderQueue(3000);

    auto carMaterial = std::make_shared<Material>(fragmentLitShader);
    carMaterial->SetTexture("_Albedo", carAlbedo);
    carMaterial->SetTexture("_NormalMap", carNormal);
    carMaterial->SetFloat("_Smoothness", 20);

    // init gameObjects
    auto rotatingCube      = GameObject::Create("Rotating Cube");
    rotatingCube->Renderer = std::make_shared<MeshRenderer>(rotatingCube, cubeMesh, fragmentLitBrickMaterial);
    m_RotatingCube = rotatingCube;

    auto rotatingCylinder      = GameObject::Create("Rotating Cylinder");
    rotatingCylinder->Renderer = std::make_shared<MeshRenderer>(rotatingCylinder, cylinderMesh, fragmentLitMaterial);
    rotatingCylinder->SetLocalPosition(Vector3(0, -3, 4));
    rotatingCylinder->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder1 = rotatingCylinder;

    auto cylinderFragmentLit      = GameObject::Create("Cylinder");
    cylinderFragmentLit->Renderer = std::make_shared<MeshRenderer>(cylinderFragmentLit, cylinderMesh, fragmentLitMaterial);
    cylinderFragmentLit->SetLocalPosition(Vector3(-3, -3, 6));
    cylinderFragmentLit->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder2 = cylinderFragmentLit;

    auto floorVertexLit      = GameObject::Create("Floor");
    floorVertexLit->Renderer = std::make_shared<MeshRenderer>(floorVertexLit, cubeMesh, fragmentLitMaterial);
    floorVertexLit->SetLocalPosition(Vector3(3, -5, 5.5f));
    floorVertexLit->SetLocalRotation(Quaternion::AngleAxis(-10, Vector3(0, 1, 0)));
    floorVertexLit->SetLocalScale(Vector3(5, 1, 2));

    auto floorFragmentLit      = GameObject::Create("Bricks");
    floorFragmentLit->Renderer = std::make_shared<MeshRenderer>(floorFragmentLit, cubeMesh, fragmentLitBrickMaterial);
    floorFragmentLit->SetLocalPosition(Vector3(-9, -5, 5.5f));
    floorFragmentLit->SetLocalRotation(Quaternion::AngleAxis(10, Vector3(0, 1, 0)));
    floorFragmentLit->SetLocalScale(Vector3(5, 1, 2));

    auto water      = GameObject::Create("Water");
    water->Renderer = std::make_shared<MeshRenderer>(water, planeMesh, m_WaterMaterial);
    water->SetLocalPosition(Vector3(0, -10, 10));
    water->SetLocalScale(Vector3(20, 1, 20));

    auto car      = GameObject::Create("Car");
    car->Renderer = std::make_shared<MeshRenderer>(car, carMesh, carMaterial);
    car->SetLocalPosition(Vector3 {11, -8.5f, -2});
    car->SetLocalRotation(Quaternion::AngleAxis(135, Vector3 {0, -1, 0}));
    car->SetLocalScale(Vector3 {0.02f, 0.02f, 0.02f});

    for (int i = 0; i < 4; ++i)
    {
        auto billboard         = GameObject::Create("Billboard " + std::to_string(i));
        auto billboardRenderer = std::make_shared<BillboardRenderer>(billboard, billboardTree);
        billboardRenderer->SetSize(5);
        billboard->SetLocalPosition(Vector3 {-20.0f + 10 * i, -10, 20});
        billboard->Renderer = std::move(billboardRenderer);
    }

    for (int i = 0; i < 3; ++i)
    {
        auto transparentCube      = GameObject::Create("Transparent Cube " + std::to_string(i));
        transparentCube->Renderer = std::make_shared<MeshRenderer>(transparentCube, cubeMesh, transparentMaterial);
        transparentCube->SetLocalPosition(Vector3(-10.0f + 5 * i, -5, 12));
        transparentCube->SetLocalRotation(Quaternion::AngleAxis(30, {0, 1, 0}));
    }

    // init lights
    m_DirectionalLight            = std::make_shared<Light>();
    m_DirectionalLight->Position  = Vector3(0, -0.3f, -1);
    m_DirectionalLight->Rotation  = Quaternion::AngleAxis(-150, Vector3(0, 1, 0)) * Quaternion::AngleAxis(30, Vector3(1, 0, 0));
    m_DirectionalLight->Intensity = Vector3(0.2f, 0.2f, 0.2f);
    m_DirectionalLight->Type      = LightType::DIRECTIONAL;

    auto pointLight         = std::make_shared<Light>();
    pointLight->Position    = Vector3(-3, -3, 4);
    pointLight->Intensity   = Vector3(1, 0, 0);
    pointLight->Attenuation = 0.3f;
    pointLight->Type        = LightType::POINT;

    m_SpotLight              = std::make_shared<Light>();
    m_SpotLight->Intensity   = Vector3(1, 1, 1);
    m_SpotLight->Attenuation = 0.05f;
    m_SpotLight->CutOffAngle = 15;
    m_SpotLight->Type        = LightType::SPOT;

    auto spotLight2         = std::make_shared<Light>();
    spotLight2->Position    = Vector3(-9, 5, 5.5f);
    spotLight2->Rotation    = Quaternion::AngleAxis(90, Vector3(1, 0, 0));
    spotLight2->Intensity   = Vector3(1, 1, 1);
    spotLight2->Attenuation = 0.01f;
    spotLight2->CutOffAngle = 15;
    spotLight2->Type        = LightType::SPOT;

    AmbientLight = Vector3(0.05f, 0.05f, 0.05f);

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
    if (Input::GetKeyDown('Q'))
        exit(0);

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
    float   offset = Math::Lerp(0, 1, phase);
    Vector4 st     = Vector4(offset, offset, 3, 3);
    m_WaterMaterial->SetVector4("_AlbedoST", st);
    m_WaterMaterial->SetVector4("_NormalMapST", st);

    // animate light
    m_DirectionalLight->Rotation = Quaternion::AngleAxis(50.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * m_DirectionalLight->Rotation;
    m_SpotLight->Position        = Camera::Current->GetPosition() + Camera::Current->GetRotation() * Vector3(-3, 0, 0);
    m_SpotLight->Rotation        = Camera::Current->GetRotation();

    // gizmos
    if (Input::GetKeyDown('G'))
        m_DrawGizmos = !m_DrawGizmos;

    if (m_DrawGizmos)
    {
        for (auto it = cbegin(); it != cend(); it++)
        {
            auto go = *it;
            if (!go || !go->Renderer)
                continue;

            auto bounds = go->Renderer->GetAABB();
            Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
        }
    }
}