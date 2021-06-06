#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"
#include "GLUT/glut.h"
#include "shaders/shader_loader.h"
#include "utils/utils.h"

#include "math/vector4/vector4.h"
#include "math/matrix4x4/matrix4x4.h"
#include "math/quaternion/quaternion.h"

const float loopDuration = 3000;
const int testVertexCount = 4;
const int testTrianglesCount = 2;

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
};

int testVertexIndexes[] = {
        0, 1, 3,
        1, 2, 3,
};

const float fov = 85;
const float zNear = 0.5f;
const float zFar = 100;

matrix4x4 perspectiveMatrix;
matrix4x4 viewMatrix;

GLuint *vertexArrayObjects;
GLuint vertexBuffer;
GLuint indexBuffer;
GLuint program;

void initVertexArrayObject() {
    vertexArrayObjects = malloc(2 * sizeof(GLuint));
    glGenVertexArrays(2, vertexArrayObjects);

    for (int i = 0; i < 2; ++i) {
        glBindVertexArray(vertexArrayObjects[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * testVertexCount * 3));
    }

    glBindVertexArray(0);
}

void initVertexBuffer(const float *vertexData, const int *vertexIndexes, int vertexCount, int trianglesCount) {
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertexCount * 3 + vertexCount * 4), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * trianglesCount * 3, vertexIndexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initPerspectiveMatrix(int width, int height) {
    perspectiveMatrix = matrix4x4_zero();

    float aspect = (float) width / (float) height;
    float top = zNear + ((float) M_PI / 180 * fov / 2);
    float bottom = -top;
    float right = aspect * top;
    float left = -right;

    perspectiveMatrix.m00 = 2 * zNear / (right - left);
    perspectiveMatrix.m11 = 2 * zNear / (top - bottom);
    perspectiveMatrix.m20 = (right + left) / (right - left);
    perspectiveMatrix.m21 = (top + bottom) / (top - bottom);
    perspectiveMatrix.m22 = -(zFar + zNear) / (zFar - zNear);
    perspectiveMatrix.m23 = -1;
    perspectiveMatrix.m32 = -2 * zFar * zNear / (zFar - zNear);
}

void initViewMatrix() {
    vector4 offset = vector4_build(0, -0.5f, 0, 0);
    viewMatrix = matrix4x4_translation(&offset);
}

void initCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void initDepth() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0, 1);
}

void initProgram(int shaderCount, GLuint *shaders) {
    program = glCreateProgram();

    for (int i = 0; i < shaderCount; ++i) {
        glAttachShader(program, shaders[i]);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *logMsg = malloc(infoLogLength + 1);
        glGetProgramInfoLog(program, infoLogLength, NULL, logMsg);
        fprintf(stderr, "Program link failed\n%s", logMsg);

        free(shaders);
        free(logMsg);
        exit(1);
    }

    for (int i = 0; i < shaderCount; ++i) {
        glDetachShader(program, shaders[i]);
    }
}

vector4 calcTranslation(float phase, float z) {
    const float radius = 2;

    float xOffset = sinf(phase * 2 * (float) M_PI) * radius;
    float yOffset = cosf(phase * 2 * (float) M_PI) * radius;

    return vector4_build(xOffset, yOffset, z, 0);
}

quaternion calcRotation(float phase, int i) {
    vector4 axis = vector4_build(0, i == 0 ? 0 : 1, i == 0 ? 1 : 0, 0);
    return quaternion_angle_axis(360 * phase, &axis);
}

vector4 calcScale(float phase) {
    float scale = float_lerp(1, 3, (sinf(phase * 2 * (float) M_PI) + 1) * 0.5f);
    return vector4_build(scale, scale, scale, 1);
}

void display() {
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = (float) glutGet(GLUT_ELAPSED_TIME);

    glUseProgram(program);

    GLint mvpMatrixLocation = glGetUniformLocation(program, "mvpMatrix");
    GLint phaseLocation = glGetUniformLocation(program, "phase");

    matrix4x4 vpMatrix = matrix4x4_multiply(&perspectiveMatrix, &viewMatrix);

    for (int i = 0; i < 2; ++i) {
        float phase = fmodf(fmodf(time, loopDuration) / loopDuration + (float) i * 0.5f, 1.0f);

        glBindVertexArray(vertexArrayObjects[i]);

        vector4 translation = calcTranslation(phase, -3 * ((float) i + 1));
        quaternion rotation = calcRotation(phase, i);
        vector4 scale = calcScale(phase);

        matrix4x4 translationMatrix = matrix4x4_translation(&translation);
        matrix4x4 rotationMatrix = matrix4x4_rotation(&rotation);
        matrix4x4 scaleMatrix = matrix4x4_scale(&scale);

        matrix4x4 modelMatrix = matrix4x4_multiply(&translationMatrix, &rotationMatrix);
        modelMatrix = matrix4x4_multiply(&modelMatrix, &scaleMatrix);

        matrix4x4 mvpMatrix = matrix4x4_multiply(&vpMatrix, &modelMatrix);

        glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (const GLfloat *) &mvpMatrix);
        glUniform1f(phaseLocation, phase);

        glDrawElements(GL_TRIANGLES, testTrianglesCount * 3, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    initPerspectiveMatrix(width, height);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_3_2_CORE_PROFILE | GLUT_DEPTH);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("OpenGL");

    GLuint *shaders = malloc(sizeof(GLuint) * 2);
    shaders[0] = loadShader(GL_VERTEX_SHADER, "shaders/vert.glsl");
    shaders[1] = loadShader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
    initProgram(2, shaders);
    free(shaders);

    initVertexBuffer(
            (const float *) &testVertexData,
            (const int *) &testVertexIndexes,
            testVertexCount,
            testTrianglesCount
    );
    initVertexArrayObject();

    initCulling();
    initViewMatrix();
    initDepth();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
//    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
