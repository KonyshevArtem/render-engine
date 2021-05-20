#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"
#include "GLUT/glut.h"
#include "shaders/shader_loader.h"
#include "utils/utils.h"
#include "matrix4x4/matrix4x4.h"

const float loopDuration = 3000;
const int testVertexCount = 18;
const int testTrianglesCount = 24;

float testVertexData[] = {
        // front triangle
        0, 0.25f, -1,               // 0
        0.25f, -0.25f, -1,          // 1
        -0.25f, -0.25f, -1,         // 2
        // back triangle
        -0.25f, -0.25f, -2,         // 3
        0.25f, -0.25f, -2,          // 4
        0, 0.25f, -2,               // 5
        //bottom side
        -0.25f, -0.25f, -1,         // 6
        0.25f, -0.25f, -1,          // 7
        0.25f, -0.25f, -2,          // 8
        -0.25f, -0.25f, -2,         // 9
        // left side
        0, 0.25f, -2,               // 10
        0, 0.25f, -1,               // 11
        -0.25f, -0.25f, -1,         // 12
        -0.25f, -0.25f, -2,         // 13
        // right side
        0.25f, -0.25f, -1,          // 14
        0, 0.25f, -1,               // 15
        0, 0.25f, -2,               // 16
        0.25f, -0.25f, -2,          // 17

        // front triangle
        -1, 0.25f, -1.5f,           // 0
        -1, -0.25f, -1,             // 1
        -1, -0.25f, -2.5f,          // 2
        // back triangle
        1, -0.25f, -2.5f,           // 3
        1, -0.25f, -1,              // 4
        1, 0.25f, -1.5f,            // 5
        //bottom side
        1, -0.25f, -2.5f,           // 6
        -1, -0.25f, -2.5f,          // 7
        -1, -0.25f, -1,             // 8
        1, -0.25f, -1,              // 9
        // left side
        -1, 0.25f, -1.5f,           // 10
        -1, -0.25f, -2.5f,          // 11
        1, -0.25f, -2.5f,           // 12
        1, 0.25f, -1.5f,            // 13
        // right side
        -1, 0.25f, -1.5f,           // 14
        1, 0.25f, -1.5f,            // 15
        1, -0.25f, -1,              // 16
        -1, -0.25f, -1,             // 17

        // colors
        // front and back – red
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        // bottom - green
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,
        // sides - blue
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
};

int testVertexIndexes[] = {
        // front
        0, 1, 2,
        // back
        3, 4, 5,
        // bottom
        6, 7, 8,
        6, 7, 9,
        // left
        10, 11, 12,
        10, 12, 13,
        // right
        14, 15, 16,
        14, 16, 17
};

const float frustumScale = 1;
const float zNear = 0.5f;
const float zFar = 3;

matrix4x4 perspectiveMatrix;
matrix4x4 viewMatrix;

GLuint* vertexArrayObjects;
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * i * testVertexCount * 3));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * testVertexCount * 3 * 2));
    }

    glBindVertexArray(0);
}

void initVertexBuffer(const float *vertexData, const int *vertexIndexes, int vertexCount, int trianglesCount) {
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertexCount * 3 * 2 + vertexCount * 4), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * trianglesCount, vertexIndexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void updatePerspectiveMatrix(int width, int height){
    perspectiveMatrix.m00 = frustumScale * (float) height / (float) width;
    perspectiveMatrix.m11 = frustumScale;

    glUseProgram(program);
    GLint perspectiveMatrixLocation = glGetUniformLocation(program, "perspectiveMatrix");
    glUniformMatrix4fv(perspectiveMatrixLocation, 1, GL_FALSE, (const GLfloat *) &perspectiveMatrix);
    glUseProgram(0);
}

void initPerspectiveMatrix(int width, int height){
    perspectiveMatrix = getZeroMatrix();

    perspectiveMatrix.m22 = (zFar + zNear) / (zNear - zFar);
    perspectiveMatrix.m23 = -1;
    perspectiveMatrix.m32 = (2 * zFar * zNear) / (zNear - zFar);

    updatePerspectiveMatrix(width, height);
}

void initViewMatrix() {
    viewMatrix = getIdentityMatrix();
    viewMatrix.m31 = -0.5f;

    glUseProgram(program);
    GLint viewMatrixLocation = glGetUniformLocation(program, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, (const GLfloat *) &viewMatrix);
    glUseProgram(program);
}

void initCulling(){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void initDepth(){
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

void calcCircleOffsets(float *xOffset, float *yOffset, float phase) {
    const float radius = 0.5f;

    *xOffset = sinf(phase * 2 * M_PI) * radius;
    *yOffset = cosf(phase * 2 * M_PI) * radius;
}

void display() {
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = (float) glutGet(GLUT_ELAPSED_TIME);
    float phase = fmodf(time, loopDuration) / loopDuration;

    glUseProgram(program);

    GLint offsetLocation = glGetUniformLocation(program, "vertexOffset");
    GLint timeLocation = glGetUniformLocation(program, "time");
    GLint loopDurationLocation = glGetUniformLocation(program, "loopDuration");

    for (int i = 0; i < 2; ++i){
        glBindVertexArray(vertexArrayObjects[i]);

        float xOffset, yOffset;
        calcCircleOffsets(&xOffset, &yOffset, fmodf(phase + (float) i * 0.5f, 1.0f));

        glUniform1f(loopDurationLocation, loopDuration);
        glUniform2f(offsetLocation, xOffset, yOffset);
        glUniform1f(timeLocation, time + (float) i * loopDuration / 2.0f);

        glDrawElements(GL_TRIANGLES, testTrianglesCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    updatePerspectiveMatrix(width, height);
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

    initVertexBuffer((const float *) &testVertexData, (const int *) &testVertexIndexes, testVertexCount, testTrianglesCount);
    initVertexArrayObject();

    initCulling();
    initPerspectiveMatrix(1024, 720);
    initViewMatrix();
    initDepth();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
//    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
