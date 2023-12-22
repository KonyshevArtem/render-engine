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

#if OPENGL_STUDY_EDITOR
bool drawGizmos = false;
int debugBaseMipLevel = 0;
std::vector<std::shared_ptr<Texture>> textures;
#endif

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
    auto brickTexture  = Texture2D::Load("resources/textures/brick");
    auto brickNormal   = Texture2D::Load("resources/textures/brick_normal");
    auto waterTexture  = Texture2D::Load("resources/textures/water");
    auto waterNormal   = Texture2D::Load("resources/textures/water_normal");
    auto billboardTree = Texture2D::Load("resources/textures/billboard_tree");
    auto windowTexture = Texture2D::Load("resources/textures/window_cube");
    auto carAlbedo     = Texture2D::Load("resources/textures/car/car_albedo");
    auto carNormal     = Texture2D::Load("resources/textures/car/car_normal");
    auto carData       = Texture2D::Load("resources/textures/car/car_data");

#ifdef OPENGL_STUDY_EDITOR
    textures.push_back(brickTexture);
    textures.push_back(brickNormal);
    textures.push_back(waterTexture);
    textures.push_back(waterNormal);
    textures.push_back(billboardTree);
    textures.push_back(windowTexture);
    textures.push_back(carAlbedo);
    textures.push_back(carNormal);
    textures.push_back(carData);
#endif

    // init skybox cubemap
    Skybox = Cubemap::Load("resources/textures/skybox/x_positive",
                           "resources/textures/skybox/x_negative",
                           "resources/textures/skybox/y_positive",
                           "resources/textures/skybox/y_negative",
                           "resources/textures/skybox/z_positive",
                           "resources/textures/skybox/z_negative");

    // init shaders
    auto standardOpaqueShader = Shader::Load("resources/shaders/standard/standard.shader", {"_REFLECTION", "_RECEIVE_SHADOWS", "_NORMAL_MAP"});
    auto standardOpaqueDataMapShader = Shader::Load("resources/shaders/standard/standard.shader", {"_DATA_MAP", "_REFLECTION", "_RECEIVE_SHADOWS", "_NORMAL_MAP"});
    auto standardTransparentShader = Shader::Load("resources/shaders/standard/standard_transparent.shader", {"_RECEIVE_SHADOWS"});
    auto standardInstancingShader = Shader::Load("resources/shaders/standard/standard.shader", {"_REFLECTION", "_RECEIVE_SHADOWS", "_NORMAL_MAP", "_INSTANCING"});

    // init meshes
    auto cubeAsset     = FBXAsset::Load("resources/models/cube.fbx");
    auto cylinderAsset = FBXAsset::Load("resources/models/cylinder.fbx");
    auto planeAsset    = FBXAsset::Load("resources/models/plane.fbx");
    auto carAsset      = FBXAsset::Load("resources/models/car.fbx");
    auto sphereAsset   = FBXAsset::Load("resources/models/sphere.fbx");

    auto cubeMesh     = cubeAsset->GetMesh(0);
    auto cylinderMesh = cylinderAsset->GetMesh(0);
    auto planeMesh    = planeAsset->GetMesh(0);
    auto carMesh      = carAsset->GetMesh(0);
    auto sphereMesh   = sphereAsset->GetMesh(0);

    // init materials
    auto standardOpaqueMaterial = std::make_shared<Material>(standardOpaqueShader);
    standardOpaqueMaterial->SetFloat("_Roughness", 0.5f);
    standardOpaqueMaterial->SetFloat("_Metallness", 1);

    auto brickMaterial = std::make_shared<Material>(standardOpaqueShader);
    brickMaterial->SetTexture("_Albedo", brickTexture);
    brickMaterial->SetTexture("_NormalMap", brickNormal);
    brickMaterial->SetFloat("_Roughness", 0.5f);
    brickMaterial->SetFloat("_Metallness", 0);
    brickMaterial->SetFloat("_NormalIntensity", 3);

    m_WaterMaterial = std::make_shared<Material>(standardOpaqueShader);
    m_WaterMaterial->SetTexture("_Albedo", waterTexture);
    m_WaterMaterial->SetTexture("_NormalMap", waterNormal);
    m_WaterMaterial->SetTexture("_ReflectionCube", Skybox);
    m_WaterMaterial->SetFloat("_ReflectionCubeLevels", static_cast<float>(Skybox->GetMipLevels()));
    m_WaterMaterial->SetFloat("_Roughness", 0.1f);
    m_WaterMaterial->SetFloat("_Metallness", 0.2f);
    m_WaterMaterial->SetFloat("_NormalIntensity", 3);

    auto transparentMaterial = std::make_shared<Material>(standardTransparentShader);
    transparentMaterial->SetTexture("_Albedo", windowTexture);
    transparentMaterial->SetFloat("_Roughness", 0.8f);
    transparentMaterial->SetFloat("_Metallness", 0);
    transparentMaterial->SetRenderQueue(3000);

    auto carMaterial = std::make_shared<Material>(standardOpaqueDataMapShader);
    carMaterial->SetTexture("_Albedo", carAlbedo);
    carMaterial->SetTexture("_NormalMap", carNormal);
    carMaterial->SetTexture("_Data", carData);
    carMaterial->SetTexture("_ReflectionCube", Skybox);
    carMaterial->SetFloat("_ReflectionCubeLevels", static_cast<float>(Skybox->GetMipLevels()));
    carMaterial->SetFloat("_NormalIntensity", 1);

    auto sphereMaterial = std::make_shared<Material>(standardInstancingShader);

    // init gameObjects
    auto rotatingCube      = GameObject::Create("Rotating Cube");
    rotatingCube->Renderer = std::make_shared<MeshRenderer>(rotatingCube, cubeMesh, brickMaterial);
    m_RotatingCube         = rotatingCube;

    auto rotatingCylinder      = GameObject::Create("Rotating Cylinder");
    rotatingCylinder->Renderer = std::make_shared<MeshRenderer>(rotatingCylinder, cylinderMesh, standardOpaqueMaterial);
    rotatingCylinder->SetLocalPosition(Vector3(0, -3, 4));
    rotatingCylinder->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder1 = rotatingCylinder;

    auto cylinderFragmentLit      = GameObject::Create("Cylinder");
    cylinderFragmentLit->Renderer = std::make_shared<MeshRenderer>(cylinderFragmentLit, cylinderMesh, standardOpaqueMaterial);
    cylinderFragmentLit->SetLocalPosition(Vector3(-3, -3, 6));
    cylinderFragmentLit->SetLocalScale(Vector3(2, 1, 0.5f));
    m_RotatingCylinder2 = cylinderFragmentLit;

    auto floorVertexLit      = GameObject::Create("Floor");
    floorVertexLit->Renderer = std::make_shared<MeshRenderer>(floorVertexLit, cubeMesh, standardOpaqueMaterial);
    floorVertexLit->SetLocalPosition(Vector3(3, -5, 5.5f));
    floorVertexLit->SetLocalRotation(Quaternion::AngleAxis(-10, Vector3(0, 1, 0)));
    floorVertexLit->SetLocalScale(Vector3(5, 1, 2));

    auto floorFragmentLit      = GameObject::Create("Bricks");
    floorFragmentLit->Renderer = std::make_shared<MeshRenderer>(floorFragmentLit, cubeMesh, brickMaterial);
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

    constexpr int spheresCount = 500;
    constexpr int gridSize     = 20;
    for (int i = 0; i < spheresCount; ++i)
    {
        auto  sphere     = GameObject::Create("Sphere " + std::to_string(i));
        float x          = (i % (gridSize * gridSize)) % gridSize;
        int   y          = i / (gridSize * gridSize);
        float z          = (i % (gridSize * gridSize)) / gridSize;
        sphere->Renderer = std::make_shared<MeshRenderer>(sphere, sphereMesh, sphereMaterial);
        sphere->SetLocalPosition({-40.0f + 80.0f * x / gridSize, -15.0f - 2.5f * y, -40.0f + 80.0f * z / gridSize});
        sphere->Renderer->CastShadows = false;
    }

    // init lights
    m_DirectionalLight            = std::make_shared<Light>();
    m_DirectionalLight->Position  = Vector3(0, -0.3f, -1);
    m_DirectionalLight->Rotation  = Quaternion::AngleAxis(-150, Vector3(0, 1, 0)) * Quaternion::AngleAxis(30, Vector3(1, 0, 0));
    m_DirectionalLight->Intensity = Vector3(0.5f, 0.5f, 0.5f);
    m_DirectionalLight->Type      = LightType::DIRECTIONAL;

    auto pointLight         = std::make_shared<Light>();
    pointLight->Position    = Vector3(-3, -3, 4);
    pointLight->Intensity   = Vector3(1, 0, 0);
    pointLight->Attenuation = 0.3f;
    pointLight->Type        = LightType::POINT;

    m_SpotLight              = std::make_shared<Light>();
    m_SpotLight->Intensity   = Vector3(1, 1, 1);
    m_SpotLight->Attenuation = 0.005f;
    m_SpotLight->CutOffAngle = 15;
    m_SpotLight->Type        = LightType::SPOT;

    auto spotLight2         = std::make_shared<Light>();
    spotLight2->Position    = Vector3(-9, 5, 5.5f);
    spotLight2->Rotation    = Quaternion::AngleAxis(90, Vector3(1, 0, 0));
    spotLight2->Intensity   = Vector3(1, 1, 1);
    spotLight2->Attenuation = 0.01f;
    spotLight2->CutOffAngle = 15;
    spotLight2->Type        = LightType::SPOT;

    AmbientLight = Vector3(0.1f, 0.1f, 0.1f);

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
    if (Input::GetKeyDown('q'))
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
    m_WaterMaterial->SetVector("_Albedo_ST", Vector4(offset, offset, 3, 3));
    m_WaterMaterial->SetVector("_NormalMap_ST", Vector4(offset, offset, 3, 3));

    // animate light
    m_DirectionalLight->Rotation = Quaternion::AngleAxis(50.0f * Time::GetDeltaTime(), Vector3 {0, 1, 0}) * m_DirectionalLight->Rotation;
    m_SpotLight->Position        = Camera::Current->GetPosition() + Camera::Current->GetRotation() * Vector3(-3, 0, 0);
    m_SpotLight->Rotation        = Camera::Current->GetRotation();

// gizmos and cheats
#if OPENGL_STUDY_EDITOR
    if (Input::GetKeyDown('g'))
        drawGizmos = !drawGizmos;

    if (drawGizmos)
    {
        for (auto it = cbegin(); it != cend(); it++)
        {
            const auto &go = *it;
            if (!go || !go->Renderer)
                continue;

            auto bounds = go->Renderer->GetAABB();
            Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
        }
    }

    int baseMipLevel = debugBaseMipLevel;
    if (Input::GetKeyDown('p'))
        ++baseMipLevel;
    if (Input::GetKeyDown('o'))
        baseMipLevel = std::max(baseMipLevel - 1, 0);

    if (baseMipLevel != debugBaseMipLevel)
    {
        debugBaseMipLevel = baseMipLevel;

        Skybox->SetBaseMipLevel(debugBaseMipLevel);
        for (const auto& t: textures)
        {
            t->SetBaseMipLevel(debugBaseMipLevel);
        }
    }
#endif
}