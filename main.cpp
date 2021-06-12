#include <cmath>
#include <cstdio>

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#include "shaders/shader_loader.h"
#include "utils/utils.h"

#include "math/math_utils.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"
#include "math/vector4/vector4.h"

const float loopDuration       = 3000;
const int   testVertexCount    = 24;
const int   testTrianglesCount = 12;

// clang-format off
float testVertexData[] = {
        // front
        -1, 1, 1,     // 0
        1, 1, 1,      // 1
        1, -1, 1,     // 2
        -1, -1, 1,    // 3

        // back
        -1, 1, -1,      // 4
        1, 1, -1,       // 5
        1, -1, -1,      // 6
        -1, -1, -1,     // 7

        // left
        -1, 1, 1,      // 8
        -1, 1, -1,     // 9
        -1, -1, -1,    // 10
        -1, -1, 1,     // 11

        // right
        1, 1, 1,      // 12
        1, 1, -1,     // 13
        1, -1, -1,    // 14
        1, -1, 1,     // 15

        // top
        1, 1, 1,      // 16
        1, 1, -1,     // 17
        -1, 1, -1,    // 18
        -1, 1, 1,     // 19

        // bottom
        1, -1, 1,      // 20
        1, -1, -1,     // 21
        -1, -1, -1,    // 22
        -1, -1, 1,     // 23

        // colors
        // front
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,

        // back
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,

        // left
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,

        // right
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,

        // top
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,

        // bottom
        0, 1, 1, 1,
        0, 1, 1, 1,
        0, 1, 1, 1,
        0, 1, 1, 1,

        // normals
        // front
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,

        // back
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,

        // left
        -1, 0, 0,
        -1, 0, 0,
        -1, 0, 0,
        -1, 0, 0,

        // right
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,

        // top
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        // bottom
        0, -1, 0,
        0, -1, 0,
        0, -1, 0,
        0, -1, 0,
};

int testVertexIndexes[] = {
        // front
        0, 1, 3,
        1, 2, 3,

        // back
        5, 4, 7,
        7, 6, 5,

        // left
        9, 8, 10,
        8, 11, 10,

        // right
        12, 13, 14,
        12, 14, 15,

        // top
        16, 19, 18,
        16, 18, 17,

        // bottom
        20, 21, 22,
        20, 22, 23
};
// clang-format on

const float fov   = 85;
const float zNear = 0.5f;
const float zFar  = 100;

Matrix4x4 perspectiveMatrix;
Matrix4x4 viewMatrix;

GLuint *vertexArrayObjects;
GLuint  vertexBuffer;
GLuint  indexBuffer;
GLuint  program;
GLuint  matricesUniformBuffer;
GLuint  lightingUniformBuffer;

void initVertexArrayObject()
{
    vertexArrayObjects = new GLuint[2];
    glGenVertexArrays(2, vertexArrayObjects);

    for (int i = 0; i < 2; ++i)
    {
        glBindVertexArray(vertexArrayObjects[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * testVertexCount * 3));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * (testVertexCount * 3 + testVertexCount * 4)));
    }

    glBindVertexArray(0);
}

void initVertexBuffer(const float *vertexData, const int *vertexIndexes, int vertexCount, int trianglesCount)
{
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertexCount * 3 * 2 + vertexCount * 4), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * trianglesCount * 3, vertexIndexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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
    viewMatrix = Matrix4x4::Translation(Vector4(0, -0.5f, 0, 0));

    long size = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, size, size, &viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void initLighting()
{
    float   directionalLightDirection[] = {-1, -1, -1};
    Vector4 directionalLightColor       = Vector4(1, 1, 1, 1);
    Vector4 ambientLight                = Vector4(0.2f, 0.2f, 0.2f, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, &directionalLightDirection);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 3, sizeof(Vector4), &directionalLightColor);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 3 + sizeof(Vector4), sizeof(Vector4), &ambientLight);
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

Vector4 calcTranslation(float phase, float z)
{
    const float radius = 2;

    float xOffset = sinf(phase * 2 * (float) M_PI) * radius;
    float yOffset = cosf(phase * 2 * (float) M_PI) * radius;

    return {xOffset, yOffset, z, 0};
}

Quaternion calcRotation(float phase, int i)
{
    Vector4 axis = Vector4(i == 0 ? 1 : 0, i == 0 ? 0 : 1, 0, 0);
    return Quaternion::AngleAxis(360 * phase, axis);
}

Vector4 calcScale(float phase)
{
    float scale = Math::Lerp(1, 2, (sinf(phase * 2 * (float) M_PI) + 1) * 0.5f);
    return {scale, scale, scale, 1};
}

void display()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto time = (float) glutGet(GLUT_ELAPSED_TIME);

    glUseProgram(program);

    GLint modelMatrixLocation = glGetUniformLocation(program, "modelMatrix");

    for (int i = 0; i < 2; ++i)
    {
        float phase = fmodf(fmodf(time, loopDuration) / loopDuration + (float) i * 0.5f, 1.0f);

        glBindVertexArray(vertexArrayObjects[i]);

        Matrix4x4 modelMatrix = Matrix4x4::TRS(
                calcTranslation(phase, -5 * ((float) i + 1)),
                calcRotation(phase, i),
                calcScale(phase));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);

        glDrawElements(GL_TRIANGLES, testTrianglesCount * 3, GL_UNSIGNED_INT, nullptr);
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

    initVertexBuffer(
            (const float *) &testVertexData,
            (const int *) &testVertexIndexes,
            testVertexCount,
            testTrianglesCount);
    initVertexArrayObject();
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
