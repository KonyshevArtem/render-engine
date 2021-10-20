#include "test_scene.h"
#include "../core/camera/camera.h"
#include "../core/cubemap/cubemap.h"
#include "../core/fbx_asset/fbx_asset.h"
#include "../core/gameObject/gameObject.h"
#include "../core/input/input.h"
#include "../core/light/light.h"
#include "../core/material/material.h"
#include "../core/mesh/mesh.h"
#include "../core/shader/shader.h"
#include "../core/texture_2d/texture_2d.h"
#include "../core/time/time.h" // NOLINT(modernize-deprecated-headers)
#include "../math/math_utils.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"
#include <cmath>
#include <memory>

using namespace std;

void TestScene::Load()
{
    auto scene = make_shared<TestScene>();
    scene->Init();
    Scene::Current = scene;
}

void TestScene::Init()
{
    // init textures
    auto brickTexture = Texture2D::Load("resources/textures/brick.png");
    auto brickNormal  = Texture2D::Load("resources/textures/brick_normal.png", false);
    auto waterTexture = Texture2D::Load("resources/textures/water.png");
    auto waterNormal  = Texture2D::Load("resources/textures/water_normal.png", false);

    // init skybox cubemap
    Skybox = Cubemap::Load("resources/textures/skybox/x_positive.png",
                           "resources/textures/skybox/x_negative.png",
                           "resources/textures/skybox/y_positive.png",
                           "resources/textures/skybox/y_negative.png",
                           "resources/textures/skybox/z_positive.png",
                           "resources/textures/skybox/z_negative.png");

    // init shaders
    auto vertexLitShader   = Shader::Load("resources/shaders/standard.glsl", vector<string> {"_VERTEX_LIGHT"});
    auto fragmentLitShader = Shader::Load("resources/shaders/standard.glsl", vector<string> {"_SMOOTHNESS", "_RECEIVE_SHADOWS", "_NORMAL_MAP"});

    // init meshes
    auto cubeAsset     = FBXAsset::Load("resources/models/cube.fbx");
    auto cylinderAsset = FBXAsset::Load("resources/models/cylinder.fbx");
    auto planeAsset    = FBXAsset::Load("resources/models/plane.fbx");

    auto cubeMesh     = cubeAsset->Meshes[0];
    auto cylinderMesh = cylinderAsset->Meshes[0];
    auto planeMesh    = planeAsset->Meshes[0];

    // init materials
    auto vertexLitMaterial = make_shared<Material>(vertexLitShader);

    auto vertexLitBrickMaterial = make_shared<Material>(vertexLitShader);
    vertexLitBrickMaterial->SetTexture("_Albedo", brickTexture);

    auto fragmentLitMaterial = make_shared<Material>(fragmentLitShader);
    fragmentLitMaterial->SetFloat("_Smoothness", 50);

    auto fragmentLitBrickMaterial = make_shared<Material>(fragmentLitShader);
    fragmentLitBrickMaterial->SetTexture("_Albedo", brickTexture);
    fragmentLitBrickMaterial->SetTexture("_NormalMap", brickNormal);
    fragmentLitBrickMaterial->SetFloat("_Smoothness", 10);

    m_WaterMaterial = make_shared<Material>(fragmentLitShader);
    m_WaterMaterial->SetTexture("_Albedo", waterTexture);
    m_WaterMaterial->SetTexture("_NormalMap", waterNormal);
    m_WaterMaterial->SetFloat("_Smoothness", 20);

    // init gameObjects
    auto rotatingCube      = make_shared<GameObject>();
    rotatingCube->Mesh     = cubeMesh;
    rotatingCube->Material = vertexLitBrickMaterial;

    auto rotatingCylinder           = make_shared<GameObject>();
    rotatingCylinder->Mesh          = cylinderMesh;
    rotatingCylinder->Material      = vertexLitMaterial;
    rotatingCylinder->LocalPosition = Vector3(0, -3, -4);
    rotatingCylinder->LocalScale    = Vector3(2, 1, 0.5f);

    auto cylinderFragmentLit           = make_shared<GameObject>();
    cylinderFragmentLit->Mesh          = cylinderMesh;
    cylinderFragmentLit->Material      = fragmentLitMaterial;
    cylinderFragmentLit->LocalPosition = Vector3(-3, -3, -6);
    cylinderFragmentLit->LocalScale    = Vector3(2, 1, 0.5f);

    auto floorVertexLit           = make_shared<GameObject>();
    floorVertexLit->Mesh          = cubeMesh;
    floorVertexLit->Material      = vertexLitMaterial;
    floorVertexLit->LocalPosition = Vector3(3, -5, -5.5f);
    floorVertexLit->LocalRotation = Quaternion::AngleAxis(10, Vector3(0, 1, 0));
    floorVertexLit->LocalScale    = Vector3(5, 1, 2);

    auto floorFragmentLit           = make_shared<GameObject>();
    floorFragmentLit->Mesh          = cubeMesh;
    floorFragmentLit->Material      = fragmentLitBrickMaterial;
    floorFragmentLit->LocalPosition = Vector3(-9, -5, -5.5f);
    floorFragmentLit->LocalRotation = Quaternion::AngleAxis(-10, Vector3(0, 1, 0));
    floorFragmentLit->LocalScale    = Vector3(5, 1, 2);

    auto water           = make_shared<GameObject>();
    water->Mesh          = planeMesh;
    water->Material      = m_WaterMaterial;
    water->LocalPosition = Vector3(0, -10, -10);
    water->LocalScale    = Vector3(20, 1, 20);

    GameObjects.push_back(rotatingCube);
    GameObjects.push_back(rotatingCylinder);
    GameObjects.push_back(cylinderFragmentLit);
    GameObjects.push_back(floorVertexLit);
    GameObjects.push_back(floorFragmentLit);
    GameObjects.push_back(water);

    // init lights
    auto dirLight       = make_shared<Light>();
    dirLight->Position  = Vector3(0, -0.3f, 1);
    dirLight->Rotation  = Quaternion::AngleAxis(180, Vector3(0, 1, 0)) * Quaternion::AngleAxis(30, Vector3(-1, 0, 0));
    dirLight->Intensity = Vector4(0.7f, 0.7f, 0.7f, 1);
    dirLight->Type      = DIRECTIONAL;

    auto pointLight         = make_shared<Light>();
    pointLight->Position    = Vector3(-3, -3, -4);
    pointLight->Intensity   = Vector4(1, 0, 0, 1);
    pointLight->Attenuation = 0.3f;
    pointLight->Type        = POINT;

    m_SpotLight              = make_shared<Light>();
    m_SpotLight->Intensity   = Vector4(1, 1, 1, 1);
    m_SpotLight->Attenuation = 0.05f;
    m_SpotLight->CutOffAngle = 15;
    m_SpotLight->Type        = SPOT;

    auto spotLight2         = make_shared<Light>();
    spotLight2->Position    = Vector3(-9, 5, -5.5f);
    spotLight2->Rotation    = Quaternion::AngleAxis(90, Vector3(-1, 0, 0));
    spotLight2->Intensity   = Vector4(1, 1, 1, 1);
    spotLight2->Attenuation = 0.01f;
    spotLight2->CutOffAngle = 15;
    spotLight2->Type        = SPOT;

    AmbientLight = Vector4(0.05f, 0.05f, 0.05f, 1);

    Lights.push_back(dirLight);
    Lights.push_back(pointLight);
    Lights.push_back(m_SpotLight);
    Lights.push_back(spotLight2);

    // init camera
    Camera::Current->Position = Vector3(-10, 0.5f, 5);
    m_CameraFlyControl        = make_unique<CameraFlyController>();
}

Vector3 TestScene::CalcTranslation(float _phase)
{
    const float radius = 2;

    float xOffset = sinf(_phase * 2 * (float) M_PI) * radius;
    float yOffset = cosf(_phase * 2 * (float) M_PI) * radius;

    return {xOffset, yOffset, -5};
}

Quaternion TestScene::CalcRotation(float _phase, int _i)
{
    Vector3 axis = Vector3(_i == 0 ? 1 : 0, _i == 0 ? 0 : 1, 0);
    return Quaternion::AngleAxis(360 * _phase, axis);
}

Vector3 TestScene::CalcScale(float _phase)
{
    float scale = Math::Lerp(1, 2, (sinf(_phase * 2 * (float) M_PI) + 1) * 0.5f);
    return {scale, scale, scale};
}

void TestScene::UpdateInternal()
{
    if (Input::GetKeyDown('q'))
        exit(0);

    m_CameraFlyControl->Update();

    float phase = fmodf(fmodf(Time::TimePassed, LOOP_DURATION) / LOOP_DURATION, 1.0f);

    GameObjects[0]->LocalPosition = CalcTranslation(phase);
    GameObjects[0]->LocalRotation = CalcRotation(phase, 0);
    GameObjects[0]->LocalScale    = CalcScale(phase);

    GameObjects[1]->LocalRotation = CalcRotation(phase, 1);

    GameObjects[2]->LocalRotation = CalcRotation(phase, 1);

    // animateWater
    float   offset = Math::Lerp(0, 1, phase);
    Vector4 st     = Vector4(offset, offset, 3, 3);
    m_WaterMaterial->SetVector4("_AlbedoST", st);
    m_WaterMaterial->SetVector4("_NormalMapST", st);

    m_SpotLight->Position = Camera::Current->Position + Camera::Current->Rotation * Vector3(-3, 0, 0);
    m_SpotLight->Rotation = Camera::Current->Rotation;
}