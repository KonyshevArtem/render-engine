#include <cmath>
#include <unordered_set>
#include <vector>

#define MATRICES_UNIFORM_SIZE 128
#define LIGHTING_UNIFORM_SIZE 176
#define CAMERA_DATA_UNIFORM_SIZE 16

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "utils/utils.h"

#include "core/gameObject/gameObject.h"
#include "core/light/light_data.h"
#include "core/mesh/cube/cube_mesh.h"
#include "core/mesh/cylinder/cylinder_mesh.h"
#include "core/mesh/mesh.h"
#include "core/shader/shader.h"
#include "core/texture/texture.h"
#include "math/math_utils.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector3/vector3.h"
#include "math/vector4/vector4.h"

GLuint matricesUniformBuffer;
GLuint lightingUniformBuffer;
GLuint cameraDataUniformBuffer;

Vector3 mouseCoord = Vector3::Zero();
Vector3 mouseDelta = Vector3::Zero();
float   prevDisplayTime;

GameObject *camera;
Vector3     cameraEulerAngles;
const float cameraRotSpeed  = 0.005f;
const float cameraMoveSpeed = 0.1f;

std::vector<GameObject *>         gameObjects;
std::unordered_set<unsigned char> inputs;

void initUniformBlocks()
{
    glGenBuffers(1, &matricesUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, MATRICES_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &lightingUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, LIGHTING_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &cameraDataUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, CAMERA_DATA_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, matricesUniformBuffer, 0, MATRICES_UNIFORM_SIZE);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightingUniformBuffer, 0, LIGHTING_UNIFORM_SIZE);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, cameraDataUniformBuffer, 0, CAMERA_DATA_UNIFORM_SIZE);
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

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4), &perspectiveMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initCameraData()
{
    Matrix4x4 viewMatrix = Matrix4x4::Rotation(camera->LocalRotation.Inverse()) * Matrix4x4::Translation(-camera->LocalPosition);

    long size = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, size, size, &viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Vector4), &(camera->LocalPosition));
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
    auto time       = (float) glutGet(GLUT_ELAPSED_TIME);
    auto deltaTime  = time - prevDisplayTime;
    prevDisplayTime = time;

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


    const float loopDuration = 3000;
    float       phase        = fmodf(fmodf(time, loopDuration) / loopDuration, 1.0f);

    gameObjects[0]->LocalPosition = calcTranslation(phase);
    gameObjects[0]->LocalRotation = calcRotation(phase, 0);
    gameObjects[0]->LocalScale    = calcScale(phase);

    gameObjects[1]->LocalRotation = calcRotation(phase, 1);

    gameObjects[2]->LocalRotation = calcRotation(phase, 1);
}

void display()
{
    update();

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initCameraData();

    for (auto go: gameObjects)
    {
        if (go->Mesh == nullptr || go->Shader == nullptr)
            continue;

        glUseProgram(go->Shader->Program);
        glBindVertexArray(go->Mesh->GetVertexArrayObject());

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        glUniformMatrix4fv(go->Shader->ModelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniformMatrix4fv(go->Shader->ModelNormalMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelNormalMatrix);
        glUniform1f(go->Shader->SmoothnessLocation, go->Smoothness);

        if (go->Texture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, go->Texture->texture);
            glUniform1i(go->Shader->AlbedoLocation, 0);
        }

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
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
    Vector3 newMouseCoord = Vector3((float) x, (float) y, 0);
    mouseDelta            = mouseCoord - newMouseCoord;
    mouseCoord            = newMouseCoord;
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

    Texture *grassTexture = Texture::Load("textures/grass.png", 800, 600);
    Texture *defaultWhite = Texture::White();

    Shader *vertexLitShader   = Shader::Load("shaders/vertexLit");
    Shader *fragmentLitShader = Shader::Load("shaders/fragmentLit");

    Mesh *cubeMesh = new CubeMesh();
    cubeMesh->Init();

    Mesh *cylinderMesh = new CylinderMesh();
    cylinderMesh->Init();

    auto *rotatingCube    = new GameObject();
    rotatingCube->Mesh    = cubeMesh;
    rotatingCube->Shader  = vertexLitShader;
    rotatingCube->Texture = grassTexture;

    auto *rotatingCylinder          = new GameObject();
    rotatingCylinder->Mesh          = cylinderMesh;
    rotatingCylinder->Shader        = vertexLitShader;
    rotatingCylinder->LocalPosition = Vector3(0, -3, -4);
    rotatingCylinder->LocalScale    = Vector3(2, 1, 0.5f);
    rotatingCylinder->Texture       = defaultWhite;

    auto *cylinderFragmentLit          = new GameObject();
    cylinderFragmentLit->Mesh          = cylinderMesh;
    cylinderFragmentLit->Shader        = fragmentLitShader;
    cylinderFragmentLit->LocalPosition = Vector3(-3, -3, -6);
    cylinderFragmentLit->LocalScale    = Vector3(2, 1, 0.5f);
    cylinderFragmentLit->Smoothness    = 5;
    cylinderFragmentLit->Texture       = defaultWhite;

    auto floorVertexLit           = new GameObject();
    floorVertexLit->Mesh          = cubeMesh;
    floorVertexLit->Shader        = vertexLitShader;
    floorVertexLit->LocalPosition = Vector3(3, -5, -5.5f);
    floorVertexLit->LocalRotation = Quaternion::AngleAxis(10, Vector3(0, 1, 0));
    floorVertexLit->LocalScale    = Vector3(5, 1, 2);
    floorVertexLit->Texture       = defaultWhite;

    auto floorFragmentLit           = new GameObject();
    floorFragmentLit->Mesh          = cubeMesh;
    floorFragmentLit->Shader        = fragmentLitShader;
    floorFragmentLit->LocalPosition = Vector3(-9, -5, -5.5f);
    floorFragmentLit->LocalRotation = Quaternion::AngleAxis(-10, Vector3(0, 1, 0));
    floorFragmentLit->LocalScale    = Vector3(5, 1, 2);
    floorFragmentLit->Smoothness    = 10;
    floorFragmentLit->Texture       = grassTexture;

    camera                = new GameObject();
    camera->LocalPosition = Vector3(-10, 0.5f, 5);

    gameObjects.push_back(rotatingCube);
    gameObjects.push_back(rotatingCylinder);
    gameObjects.push_back(cylinderFragmentLit);
    gameObjects.push_back(floorVertexLit);
    gameObjects.push_back(floorFragmentLit);

    initUniformBlocks();

    initCulling();
    initDepth();
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouseMove);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();

    return 0;
}
