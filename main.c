#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"
#include "GLUT/glut.h"
#include "shaders/shader_loader.h"
#include "utils.h"

const int testVertexCount = 3;
float testVertexData[] = {
        0, 0.25f, 0, 1.0f,
        0.25f, -0.25f, 0, 1.0f,
        -0.25f, -0.25f, 0, 1.0f,
        1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 1
};

GLuint vao;
GLuint vertexBuffer;
GLuint program;

void initVertexArrayObject() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void initVertexBuffer(const float *vertexData, int vertexCount) {
    glGenBuffers(1, &vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 4 * 2, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void offsetVertexes(const float *vertexData, int vertexCount, float xOffset, float yOffset) {
    float offsetVertexData[vertexCount * 4];
    for (int i = 0; i < vertexCount; ++i) {
        offsetVertexData[i * 4 + 0] = vertexData[i * 4 + 0] + xOffset;
        offsetVertexData[i * 4 + 1] = vertexData[i * 4 + 1] + yOffset;
        offsetVertexData[i * 4 + 2] = vertexData[i * 4 + 2];
        offsetVertexData[i * 4 + 3] = vertexData[i * 4 + 3];
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertexCount * 4, &offsetVertexData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void calcCircleOffsets(float *xOffset, float *yOffset) {
    const float period = 2000;
    const float radius = 0.5f;

    float time = (float) glutGet(GLUT_ELAPSED_TIME);
    float phase = fmodf(time, period) / period;

    *xOffset = sinf(phase * 2 * M_PI) * radius;
    *yOffset = cosf(phase * 2 * M_PI) * radius;
}

void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    float xOffset, yOffset;
    calcCircleOffsets(&xOffset, &yOffset);
    offsetVertexes((const float *) &testVertexData, 3, xOffset, yOffset);

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * 3 * 4));

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("OpenGL");

    GLuint *shaders = malloc(sizeof(GLuint) * 2);
    shaders[0] = loadShader(GL_VERTEX_SHADER, "shaders/vert.glsl");
    shaders[1] = loadShader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
    initProgram(2, shaders);
    free(shaders);

    initVertexArrayObject();
    initVertexBuffer((const float *) &testVertexData, testVertexCount);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
//    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
