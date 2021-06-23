#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "string"

class Shader
{
public:
    GLuint Program;
    GLint  ModelMatrixLocation;
    GLint  ModelNormalMatrixLocation;
    GLuint MatricesUniformIndex;
    GLuint LightingUniformIndex;

    static Shader *Load(const std::string &path);

private:
    explicit Shader(GLuint program);

    static GLuint CompileShaderPart(GLuint shaderPartType, const std::string &path);
    static GLuint LinkProgram(GLuint vertexPart, GLuint fragmentPart);
};

#endif //OPENGL_STUDY_SHADER_H
