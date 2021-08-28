#include "test_scene.h"
#include "../core/camera/camera.h"
#include "../core/mesh/cube/cube_mesh.h"
#include "../core/mesh/cylinder/cylinder_mesh.h"
#include "../core/mesh/plane/plane_mesh.h"
#include "../core/time/time.h" // NOLINT(modernize-deprecated-headers)
#include "../math/math_utils.h"
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
    auto grassTexture = Texture::Load("textures/grass.png", 800, 600);
    auto waterTexture = Texture::Load("textures/water.png", 512, 512);

    // init shaders
    auto vertexLitShader   = Shader::Load("shaders/vertexLit");
    auto fragmentLitShader = Shader::Load("shaders/fragmentLit");

    // init meshes
    auto cubeMesh = make_shared<CubeMesh>();
    cubeMesh->Init();

    auto cylinderMesh = make_shared<CylinderMesh>();
    cylinderMesh->Init();

    auto planeMesh = make_shared<PlaneMesh>();
    planeMesh->Init();

    // init materials
    auto vertexLitMaterial = make_shared<Material>(vertexLitShader);

    auto vertexLitGrassMaterial    = make_shared<Material>(vertexLitShader);
    vertexLitGrassMaterial->Albedo = grassTexture;

    auto fragmentLitMaterial        = make_shared<Material>(fragmentLitShader);
    fragmentLitMaterial->Smoothness = 50;

    auto fragmentLitGrassMaterial        = make_shared<Material>(fragmentLitShader);
    fragmentLitGrassMaterial->Albedo     = grassTexture;
    fragmentLitGrassMaterial->Smoothness = 10;

    m_WaterMaterial             = make_shared<Material>(fragmentLitShader);
    m_WaterMaterial->Albedo     = waterTexture;
    m_WaterMaterial->Smoothness = 20;

    // init gameObjects
    auto rotatingCube      = make_shared<GameObject>();
    rotatingCube->Mesh     = cubeMesh;
    rotatingCube->Material = vertexLitGrassMaterial;

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
    floorFragmentLit->Material      = fragmentLitGrassMaterial;
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
    m_CameraFlyControl->Update();

    float phase = fmodf(fmodf(Time::TimePassed, LOOP_DURATION) / LOOP_DURATION, 1.0f);

    GameObjects[0]->LocalPosition = CalcTranslation(phase);
    GameObjects[0]->LocalRotation = CalcRotation(phase, 0);
    GameObjects[0]->LocalScale    = CalcScale(phase);

    GameObjects[1]->LocalRotation = CalcRotation(phase, 1);

    GameObjects[2]->LocalRotation = CalcRotation(phase, 1);

    // animateWater
    float offset              = Math::Lerp(0, 1, phase);
    m_WaterMaterial->AlbedoST = Vector4(offset, offset, 3, 3);
}