#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

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
    GLint  AlbedoSTLocation;

    static shared_ptr<Shader> Load(const string &_path);

    ~Shader();

private:
    explicit Shader(GLuint _program);

    static GLuint CompileShaderPart(GLuint _shaderPartType, const string &_path);
    static GLuint LinkProgram(GLuint _vertexPart, GLuint _fragmentPart);
};

#endif //OPENGL_STUDY_SHADER_H