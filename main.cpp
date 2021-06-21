#include <cmath>
#include <cstdio>
#include <vector>

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "shaders/shader_loader.h"
#include "utils/utils.h"

#include "core/light/light_data.h"
#include "core/mesh/cube/cube_mesh.h"
#include "core/mesh/cylinder/cylinder_mesh.h"
#include "core/mesh/mesh.h"
#include "math/math_utils.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector3/vector3.h"
#include "math/vector4/vector4.h"

const float loopDuration = 3000;

const float fov   = 85;
const float zNear = 0.5f;
const float zFar  = 100;

Matrix4x4 perspectiveMatrix;
Matrix4x4 viewMatrix;

GLuint program;
GLuint matricesUniformBuffer;
GLuint lightingUniformBuffer;

std::vector<Mesh *> meshes;

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
    perspectiveMatrix = Matrix4x4::Zero();

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
    viewMatrix = Matrix4x4::Translation(Vector3(0, -0.5f, 0));

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

void initProgram(int shaderCount, GLuint *shaders)
{
    program = glCreateProgram();

    for (int i = 0; i < shaderCount; ++i)
        glAttachShader(program, shaders[i]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *logMsg = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, logMsg);
        fprintf(stderr, "Program link failed\n%s", logMsg);

        free(shaders);
        free(logMsg);
        exit(1);
    }

    for (int i = 0; i < shaderCount; ++i)
        glDetachShader(program, shaders[i]);
}

Vector3 calcTranslation(float phase, float z)
{
    const float radius = 2;

    float xOffset = sinf(phase * 2 * (float) M_PI) * radius;
    float yOffset = cosf(phase * 2 * (float) M_PI) * radius;

    return {xOffset, yOffset, z};
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

void display()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto time = (float) glutGet(GLUT_ELAPSED_TIME);

    glUseProgram(program);

    GLint modelMatrixLocation       = glGetUniformLocation(program, "modelMatrix");
    GLint modelNormalMatrixLocation = glGetUniformLocation(program, "modelNormalMatrix");

    for (int i = 0; i < meshes.size(); ++i)
    {
        float phase = fmodf(fmodf(time, loopDuration) / loopDuration + (float) i * 0.5f, 1.0f);

        glBindVertexArray(meshes[i]->GetVertexArrayObject());

        Matrix4x4 translation = Matrix4x4::Translation(calcTranslation(phase, -5));
        Matrix4x4 rotation    = Matrix4x4::Rotation(calcRotation(phase, i));
        Matrix4x4 scale       = Matrix4x4::Scale(calcScale(phase));

        Matrix4x4 modelMatrix = Matrix4x4::Identity();
        if (i == 0)
            modelMatrix = translation * rotation * scale;
        else
            modelMatrix = Matrix4x4::TRS(Vector3(0, -3, -4), calcRotation(phase, i), Vector3(2, 1, 0.5f));

        Matrix4x4 modelNormalMatrix = (rotation * scale).Invert().Transpose();

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniformMatrix4fv(modelNormalMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelNormalMatrix);

        glDrawElements(GL_TRIANGLES, meshes[i]->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);
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

    auto *shaders = new GLuint[2];
    shaders[0]    = ShaderLoader::LoadShader(GL_VERTEX_SHADER, "shaders/vert.glsl");
    shaders[1]    = ShaderLoader::LoadShader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
    initProgram(2, shaders);
    free(shaders);

    Mesh *cube = new CubeMesh();
    cube->Init();

    Mesh *cylinder = new CylinderMesh();
    cylinder->Init();

    meshes.push_back(cube);
    meshes.push_back(cylinder);

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
