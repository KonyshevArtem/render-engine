#include <cmath>
#include <unordered_set>
#include <vector>

#define LIGHTING_UNIFORM_SIZE 176
#define CAMERA_DATA_UNIFORM_SIZE 144

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "utils/utils.h"

#include "core/gameObject/gameObject.h"
#include "core/light/light_data.h"
#include "core/mesh/cube/cube_mesh.h"
#include "core/mesh/cylinder/cylinder_mesh.h"
#include "core/mesh/plane/plane_mesh.h"
#include "core/shader/shader.h"
#include "core/texture/texture.h"
#include "math/math_utils.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector3/vector3.h"
#include "math/vector4/vector4.h"

using namespace std;

GLuint lightingUniformBuffer;
GLuint cameraDataUniformBuffer;

Vector3 mouseCoord    = Vector3::Zero();
Vector3 mouseDelta    = Vector3::Zero();
Vector3 oldMouseCoord = Vector3::Zero();
float   prevDisplayTime;

unique_ptr<GameObject> camera;
Vector3                cameraEulerAngles;
const float            cameraRotSpeed  = 0.005f;
const float            cameraMoveSpeed = 0.1f;

vector<shared_ptr<GameObject>> gameObjects;
unordered_set<unsigned char>   inputs;

shared_ptr<Material> waterMaterial;

void initUniformBlocks()
{
    glGenBuffers(1, &lightingUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, LIGHTING_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &cameraDataUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, CAMERA_DATA_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, cameraDataUniformBuffer, 0, CAMERA_DATA_UNIFORM_SIZE);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightingUniformBuffer, 0, LIGHTING_UNIFORM_SIZE);
}

void initPerspectiveMatrix(int width, int height)
{
    Matrix4x4 perspectiveMatrix = Matrix4x4::Zero();

    const float fov   = 120;
    const float zNear = 0.5f;
    const float zFar  = 100;

    float aspect = (float) width / (float) height;
    float top    = zNear * ((float) M_PI / 180 * fov / 2);
    float bottom = -top;
    float right  = aspect * top;
    float left   = -right;

    perspectiveMatrix.m00 = 2 * zNear / (right - left);
    perspectiveMatrix.m11 = 2 * zNear / (top - bottom);
    perspectiveMatrix.m20 = (right + left) / (right - left);
    perspectiveMatrix.m21 = (top + bottom) / (top - bottom);
    perspectiveMatrix.m22 = -(zFar + zNear) / (zFar - zNear);
    perspectiveMatrix.m23 = -1;
    perspectiveMatrix.m32 = -2 * zFar * zNear / (zFar - zNear);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4), &perspectiveMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initCameraData()
{
    Matrix4x4 viewMatrix = Matrix4x4::Rotation(camera->LocalRotation.Inverse()) * Matrix4x4::Translation(-camera->LocalPosition);

    long matrixSize = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, matrixSize, matrixSize, &viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * matrixSize, sizeof(Vector4), &(camera->LocalPosition));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initLighting()
{
    const float gammaCorrection = 1.0f / 2.2f;
    const int   lightsCount     = 3;
    LightData   lights[lightsCount];

    LightData dirLight;
    dirLight.PosOrDirWS    = Vector3 {0, -0.3f, 1};
    dirLight.Intensity     = Vector4(1, 1, 1, 1);
    dirLight.IsDirectional = true;

    LightData pointLight;
    pointLight.PosOrDirWS    = Vector3(-3, -3, -4);
    pointLight.Intensity     = Vector4(1, 0, 0, 1);
    pointLight.IsDirectional = false;
    pointLight.Attenuation   = 0.3f;

    lights[0] = dirLight;
    lights[1] = pointLight;

    Vector4 ambientLight = Vector4(0.05f, 0.05f, 0.05f, 1);

    long lightDataSize = sizeof(LightData) * lightsCount;
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, lightDataSize, &lights);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize, sizeof(Vector4), &ambientLight);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize + sizeof(Vector4), sizeof(int), &lightsCount);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize + sizeof(Vector4) + sizeof(int), sizeof(float), &gammaCorrection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initCulling()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void initDepth()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0, 1);
}

Vector3 calcTranslation(float phase)
{
    const float radius = 2;

    float xOffset = sinf(phase * 2 * (float) M_PI) * radius;
    float yOffset = cosf(phase * 2 * (float) M_PI) * radius;

    return {xOffset, yOffset, -5};
}

Quaternion calcRotation(float phase, int i)
{
    Vector3 axis = Vector3(i == 0 ? 1 : 0, i == 0 ? 0 : 1, 0);
    return Quaternion::AngleAxis(360 * phase, axis);
}

Vector3 calcScale(float phase)
{
    float scale = Math::Lerp(1, 2, (sinf(phase * 2 * (float) M_PI) + 1) * 0.5f);
    return {scale, scale, scale};
}

void update()
{
    // process quit input
    if (inputs.contains('q'))
    {
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }

    // calculate correct mouse coordinate delta
    mouseDelta    = oldMouseCoord - mouseCoord;
    oldMouseCoord = mouseCoord;

    // calculate time delta from previous update
    auto time       = (float) glutGet(GLUT_ELAPSED_TIME);
    auto deltaTime  = time - prevDisplayTime;
    prevDisplayTime = time;

    // update camera rotation
    cameraEulerAngles = cameraEulerAngles + mouseDelta * cameraRotSpeed * deltaTime;
    if (cameraEulerAngles.x < 0)
        cameraEulerAngles.x = 360;
    if (cameraEulerAngles.x > 360)
        cameraEulerAngles.x = 0;
    if (cameraEulerAngles.y < 0)
        cameraEulerAngles.y = 360;
    if (cameraEulerAngles.y > 360)
        cameraEulerAngles.y = 0;

    Quaternion xRot       = Quaternion::AngleAxis(cameraEulerAngles.y, Vector3(1, 0, 0));
    Quaternion yRot       = Quaternion::AngleAxis(cameraEulerAngles.x, Vector3(0, 1, 0));
    camera->LocalRotation = yRot * xRot;

    Vector3 cameraFwd   = camera->LocalRotation * Vector3(0, 0, -1);
    Vector3 cameraRight = camera->LocalRotation * Vector3(1, 0, 0);
    if (inputs.contains('w'))
        camera->LocalPosition = camera->LocalPosition + cameraFwd * cameraMoveSpeed;
    if (inputs.contains('s'))
        camera->LocalPosition = camera->LocalPosition - cameraFwd * cameraMoveSpeed;
    if (inputs.contains('d'))
        camera->LocalPosition = camera->LocalPosition + cameraRight * cameraMoveSpeed;
    if (inputs.contains('a'))
        camera->LocalPosition = camera->LocalPosition - cameraRight * cameraMoveSpeed;

    // animate gameObjects
    const float loopDuration = 3000;
    float       phase        = fmodf(fmodf(time, loopDuration) / loopDuration, 1.0f);

    gameObjects[0]->LocalPosition = calcTranslation(phase);
    gameObjects[0]->LocalRotation = calcRotation(phase, 0);
    gameObjects[0]->LocalScale    = calcScale(phase);

    gameObjects[1]->LocalRotation = calcRotation(phase, 1);

    gameObjects[2]->LocalRotation = calcRotation(phase, 1);

    // animateWater
    float offset            = Math::Lerp(0, 1, phase);
    waterMaterial->AlbedoST = Vector4(offset, offset, 3, 3);
}

void display()
{
    update();

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initCameraData();

    for (const auto &go: gameObjects)
    {
        if (go->Mesh == nullptr || go->Material == nullptr)
            continue;

        glUseProgram(go->Material->ShaderPtr->Program);
        glBindVertexArray(go->Mesh->GetVertexArrayObject());

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        glUniformMatrix4fv(go->Material->ShaderPtr->ModelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniformMatrix4fv(go->Material->ShaderPtr->ModelNormalMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelNormalMatrix);
        glUniform1f(go->Material->ShaderPtr->SmoothnessLocation, go->Material->Smoothness);

        if (go->Material->Albedo != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, go->Material->Albedo->Ptr);
            glBindSampler(0, go->Material->Albedo->Sampler);
            glUniform1i(go->Material->ShaderPtr->AlbedoLocation, 0);
            glUniform4fv(go->Material->ShaderPtr->AlbedoSTLocation, 1, (const GLfloat *) &go->Material->AlbedoST);
        }

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindSampler(0, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    initPerspectiveMatrix(width, height);
}

void mouseMove(int x, int y)
{
    mouseCoord = Vector3((float) x, (float) y, 0);
}

void keyboardDown(unsigned char key, int x, int y)
{
    if (!inputs.contains(key))
        inputs.insert(key);
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (inputs.contains(key))
        inputs.erase(key);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_3_2_CORE_PROFILE | GLUT_DEPTH);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("OpenGL");

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
    fragmentLitMaterial->Smoothness = 5;

    auto fragmentLitGrassMaterial        = make_shared<Material>(fragmentLitShader);
    fragmentLitGrassMaterial->Albedo     = grassTexture;
    fragmentLitGrassMaterial->Smoothness = 10;

    waterMaterial             = make_shared<Material>(fragmentLitShader);
    waterMaterial->Albedo     = waterTexture;
    waterMaterial->Smoothness = 20;

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
    water->Material      = waterMaterial;
    water->LocalPosition = Vector3(0, -10, -10);
    water->LocalScale    = Vector3(20, 1, 20);

    gameObjects.push_back(rotatingCube);
    gameObjects.push_back(rotatingCylinder);
    gameObjects.push_back(cylinderFragmentLit);
    gameObjects.push_back(floorVertexLit);
    gameObjects.push_back(floorFragmentLit);
    gameObjects.push_back(water);

    // init camera
    camera                = make_unique<GameObject>();
    camera->LocalPosition = Vector3(-10, 0.5f, 5);

    initUniformBlocks();

    initCulling();
    initDepth();
    initLighting();
    glEnable(GL_FRAMEBUFFER_SRGB);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouseMove);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();

    return 0;
}
