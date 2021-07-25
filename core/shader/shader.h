#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "string"

using namespace std;

class Shader
{
public:
    GLuint Program;
    GLint  ModelMatrixLocation;
    GLint  ModelNormalMatrixLocation;
    GLint  SmoothnessLocation;
    GLint  AlbedoLocation;

    static shared_ptr<Shader> Load(const string &path);

private:
    explicit Shader(GLuint program);

    static GLuint CompileShaderPart(GLuint shaderPartType, const string &path);
    static GLuint LinkProgram(GLuint vertexPart, GLuint fragmentPart);
};

#endif //OPENGL_STUDY_SHADER_H
