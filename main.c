#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GL_SILENCE_DEPRECATION

#include "OpenGL/gl3.h"
#include "GLUT/glut.h"
#include "shaders/shader_loader.h"
#include "utils.h"

const float loopDuration = 3000;
const int testVertexCount = 24;
float testVertexData[] = {
        // front triangle
        0, 0.25f, -1, 1,
        0.25f, -0.25f, -1, 1,
        -0.25f, -0.25f, -1, 1,
        // back triangle
        -0.25f, -0.25f, -2, 1,
        0.25f, -0.25f, -2, 1,
        0, 0.25f, -2, 1,
        //bottom side
        -0.25f, -0.25f, -1, 1,
        0.25f, -0.25f, -1, 1,
        0.25f, -0.25f, -2, 1,
        -0.25f, -0.25f, -1, 1,
        0.25f, -0.25f, -2, 1,
        -0.25f, -0.25f, -2, 1,
        // left side
        0, 0.25f, -2, 1,
        0, 0.25f, -1, 1,
        -0.25f, -0.25f, -1, 1,
        -0.25f, -0.25f, -2, 1,
        0, 0.25f, -2, 1,
        -0.25f, -0.25f, -1, 1,
        // right side
        0.25f, -0.25f, -1, 1,
        0, 0.25f, -1, 1,
        0, 0.25f, -2, 1,
        0.25f, -0.25f, -1, 1,
        0, 0.25f, -2, 1,
        0.25f, -0.25f, -2, 1,

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
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
};

const float frustumScale = 1;
const float zNear = 0.5f;
const float zFar = 3;

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

void initPerspectiveMatrix(){
    float perspectiveMatrix[16];
    for (int i = 0; i < 16; ++i)
        perspectiveMatrix[i] = 0;

    perspectiveMatrix[0] = frustumScale;
    perspectiveMatrix[5] = frustumScale;
    perspectiveMatrix[10] = (zFar + zNear) / (zNear - zFar);
    perspectiveMatrix[11] = -1;
    perspectiveMatrix[14] = (2 * zFar * zNear) / (zNear - zFar);

    glUseProgram(program);
    GLint perspectiveMatrixLocation = glGetUniformLocation(program, "perspectiveMatrix");
    glUniformMatrix4fv(perspectiveMatrixLocation, 1, GL_FALSE, &perspectiveMatrix[0]);
    glUseProgram(0);
}

void initCulling(){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
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
    glClear(GL_COLOR_BUFFER_BIT);

    float time = (float) glutGet(GLUT_ELAPSED_TIME);
    float phase = fmodf(time, loopDuration) / loopDuration;

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) (sizeof(float) * testVertexCount * 4));

    GLint offsetLocation = glGetUniformLocation(program, "vertexOffset");
    GLint timeLocation = glGetUniformLocation(program, "time");
    GLint loopDurationLocation = glGetUniformLocation(program, "loopDuration");

    glUniform1f(loopDurationLocation, loopDuration);

    float xOffset, yOffset;

    // first triangle
    {
        calcCircleOffsets(&xOffset, &yOffset, phase);
        glUniform2f(offsetLocation, xOffset, yOffset);
        glUniform1f(timeLocation, time);

        glDrawArrays(GL_TRIANGLES, 0, testVertexCount);
    }

    // second triangle
    {
        calcCircleOffsets(&xOffset, &yOffset, fmodf(phase + 0.5f, 1.0f));
        glUniform2f(offsetLocation, xOffset, yOffset);
        glUniform1f(timeLocation, time + loopDuration / 2.0f);

        glDrawArrays(GL_TRIANGLES, 0, testVertexCount);
    }

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
    initPerspectiveMatrix();
    initCulling();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
//    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
