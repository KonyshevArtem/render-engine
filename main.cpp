#include <cmath>
#include <vector>

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "shaders/shader_loader.h"
#include "utils/utils.h"

#include "core/gameObject/gameObject.h"
#include "core/light/light_data.h"
#include "core/mesh/cube/cube_mesh.h"
#include "core/mesh/cylinder/cylinder_mesh.h"
#include "core/mesh/mesh.h"
#include "math/math_utils.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector3/vector3.h"
#include "math/vector4/vector4.h"

GLuint program;
GLuint matricesUniformBuffer;
GLuint lightingUniformBuffer;

std::vector<GameObject *> gameObjects;

void initUniformBlocks()
{
    GLuint matricesUniformIndex = glGetUniformBlockIndex(program, "Matrices");
    GLuint lightingUniformIndex = glGetUniformBlockIndex(program, "Lighting");

    glUniformBlockBinding(program, matricesUniformIndex, 0);
    glUniformBlockBinding(program, lightingUniformIndex, 1);

    GLint matricesSize;
    GLint lightingSize;
    glGetActiveUniformBlockiv(program, matricesUniformIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matricesSize);
    glGetActiveUniformBlockiv(program, lightingUniformIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightingSize);

    glGenBuffers(1, &matricesUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, matricesSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &lightingUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, lightingSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, matricesUniformBuffer, 0, matricesSize);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightingUniformBuffer, 0, lightingSize);
}

void initPerspectiveMatrix(int width, int height)
{
    Matrix4x4 perspectiveMatrix = Matrix4x4::Zero();

    const float fov   = 85;
    const float zNear = 0.5f;
    const float zFar  = 100;

    float aspect = (float) width / (float) height;
    float top    = zNear + ((float) M_PI / 180 * fov / 2);
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

void initViewMatrix()
{
    Matrix4x4 viewMatrix = Matrix4x4::Translation(Vector3(3, -0.5f, 0));

    long size = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, size, size, &viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initLighting()
{
    int       lightsCount = 2;
    LightData lights[lightsCount];

    LightData dirLight;
    dirLight.PosOrDirWS    = Vector3 {-1, -1, -1};
    dirLight.Intensity     = Vector4(1, 1, 1, 1);
    dirLight.IsDirectional = true;

    LightData pointLight;
    pointLight.PosOrDirWS    = Vector3(-3, -3, -4);
    pointLight.Intensity     = Vector4(1, 0, 0, 1);
    pointLight.IsDirectional = false;
    pointLight.Attenuation   = 0.3f;

    lights[0] = dirLight;
    lights[1] = pointLight;

    Vector4 ambientLight = Vector4(0.2f, 0.2f, 0.2f, 1);

    long lightDataSize = sizeof(LightData) * lightsCount;
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, lightDataSize, &lights);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize, sizeof(Vector4), &ambientLight);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize + sizeof(Vector4), sizeof(int), &lightsCount);
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
    const float loopDuration = 3000;
    auto        time         = (float) glutGet(GLUT_ELAPSED_TIME);
    float       phase        = fmodf(fmodf(time, loopDuration) / loopDuration, 1.0f);

    gameObjects[0]->LocalPosition = calcTranslation(phase);
    gameObjects[0]->LocalRotation = calcRotation(phase, 0);
    gameObjects[0]->LocalScale    = calcScale(phase);

    gameObjects[1]->LocalRotation = calcRotation(phase, 1);
}

void display()
{
    update();

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    GLint modelMatrixLocation       = glGetUniformLocation(program, "modelMatrix");
    GLint modelNormalMatrixLocation = glGetUniformLocation(program, "modelNormalMatrix");

    for (auto go: gameObjects)
    {
        if (go->Mesh == nullptr)
            continue;

        glBindVertexArray(go->Mesh->GetVertexArrayObject());

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniformMatrix4fv(modelNormalMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelNormalMatrix);

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
    glUseProgram(0);

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

    program = ShaderLoader::LoadShader("shaders/fragmentLit");

    Mesh *cubeMesh = new CubeMesh();
    cubeMesh->Init();

    Mesh *cylinderMesh = new CylinderMesh();
    cylinderMesh->Init();

    auto *rotatingCube = new GameObject();
    rotatingCube->Mesh = cubeMesh;

    auto *rotatingCylinder          = new GameObject();
    rotatingCylinder->Mesh          = cylinderMesh;
    rotatingCylinder->LocalPosition = Vector3(0, -3, -4);
    rotatingCylinder->LocalScale    = Vector3(2, 1, 0.5f);

    auto floorVertexLit           = new GameObject();
    floorVertexLit->Mesh          = cubeMesh;
    floorVertexLit->LocalPosition = Vector3(-9, -5, -5.5f);
    floorVertexLit->LocalRotation = Quaternion::AngleAxis(-10, Vector3(0, 1, 0));
    floorVertexLit->LocalScale    = Vector3(5, 1, 2);

    auto floorFragmentLit           = new GameObject();
    floorFragmentLit->Mesh          = cubeMesh;
    floorFragmentLit->LocalPosition = Vector3(3, -5, -5.5f);
    floorFragmentLit->LocalRotation = Quaternion::AngleAxis(10, Vector3(0, 1, 0));
    floorFragmentLit->LocalScale    = Vector3(5, 1, 2);

    gameObjects.push_back(rotatingCube);
    gameObjects.push_back(rotatingCylinder);
    gameObjects.push_back(floorVertexLit);
    gameObjects.push_back(floorFragmentLit);

    initUniformBlocks();

    initCulling();
    initViewMatrix();
    initDepth();
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
