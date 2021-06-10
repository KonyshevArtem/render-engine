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
const int   testVertexCount    = 4;
const int   testTrianglesCount = 2;

float lightDirection[] = {-1, 0, -1};
float lightColor[]     = {1, 0.5f, 0, 1};

// clang-format off
float testVertexData[] = {
        -1, 1, 0,     // 0
        1, 1, 0,      // 1
        1, -1, 0,     // 2
        -1, -1, 0,    // 3

        // colors
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,

        // normals
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
};

int testVertexIndexes[] = {
        0, 1, 3,
        1, 2, 3,
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
}

void initViewMatrix()
{
    viewMatrix = Matrix4x4::Translation(Vector4(0, -0.5f, 0, 0));
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
    Vector4 axis = Vector4(0, i == 0 ? 0 : 1, i == 0 ? 1 : 0, 0);
    return Quaternion::AngleAxis(360 * phase, axis);
}

Vector4 calcScale(float phase)
{
    float scale = Math::Lerp(1, 3, (sinf(phase * 2 * (float) M_PI) + 1) * 0.5f);
    return {scale, scale, scale, 1};
}

void display()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto time = (float) glutGet(GLUT_ELAPSED_TIME);

    glUseProgram(program);

    GLint projMatrixLocation     = glGetUniformLocation(program, "projMatrix");
    GLint viewMatrixLocation     = glGetUniformLocation(program, "viewMatrix");
    GLint modelMatrixLocation    = glGetUniformLocation(program, "modelMatrix");
    GLint lightDirectionLocation = glGetUniformLocation(program, "lightDirection");
    GLint lightColorLocation     = glGetUniformLocation(program, "lightColor");

    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, (const GLfloat *) &perspectiveMatrix);
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, (const GLfloat *) &viewMatrix);
    glUniform4fv(lightColorLocation, 1, (const GLfloat *) &lightColor);

    for (int i = 0; i < 2; ++i)
    {
        float phase = fmodf(fmodf(time, loopDuration) / loopDuration + (float) i * 0.5f, 1.0f);

        glBindVertexArray(vertexArrayObjects[i]);

        Matrix4x4 modelMatrix = Matrix4x4::TRS(
                calcTranslation(phase, -3 * ((float) i + 1)),
                calcRotation(phase, i),
                calcScale(phase));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniform3fv(lightDirectionLocation, 1, (const GLfloat *) &lightDirection);

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

    initCulling();
    initViewMatrix();
    initDepth();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
