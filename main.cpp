#include <cmath>

#define LIGHTING_UNIFORM_SIZE 176
#define CAMERA_DATA_UNIFORM_SIZE 144

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "utils/utils.h"

#include "core/gameObject/gameObject.h"
#include "core/input/input.h"
#include "core/light/light_data.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector3/vector3.h"
#include "math/vector4/vector4.h"
#include "scenes/test_scene.h"

using namespace std;

GLuint lightingUniformBuffer;
GLuint cameraDataUniformBuffer;

float prevDisplayTime;

unique_ptr<GameObject> camera;
Vector3                cameraEulerAngles;
const float            cameraRotSpeed  = 0.005f;
const float            cameraMoveSpeed = 0.1f;

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

void update()
{
    Input::Update();

    // process quit input
    if (Input::IsKeyDown('q'))
    {
        glutDestroyWindow(glutGetWindow());
        exit(0);
    }

    // calculate time delta from previous update
    auto time       = (float) glutGet(GLUT_ELAPSED_TIME);
    auto deltaTime  = time - prevDisplayTime;
    prevDisplayTime = time;

    // update camera rotation
    cameraEulerAngles = cameraEulerAngles + Input::GetMouseDelta() * cameraRotSpeed * deltaTime;
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
    if (Input::IsKeyDown('w'))
        camera->LocalPosition = camera->LocalPosition + cameraFwd * cameraMoveSpeed;
    if (Input::IsKeyDown('s'))
        camera->LocalPosition = camera->LocalPosition - cameraFwd * cameraMoveSpeed;
    if (Input::IsKeyDown('d'))
        camera->LocalPosition = camera->LocalPosition + cameraRight * cameraMoveSpeed;
    if (Input::IsKeyDown('a'))
        camera->LocalPosition = camera->LocalPosition - cameraRight * cameraMoveSpeed;

    if (Scene::Current != nullptr)
        Scene::Current->Update(time, deltaTime);
}

void display()
{
    update();

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initCameraData();

    for (const auto &go: Scene::Current->GameObjects)
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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_3_2_CORE_PROFILE | GLUT_DEPTH);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("OpenGL");

    Input::Init();

    TestScene::Load();

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

    glutMainLoop();

    return 0;
}
