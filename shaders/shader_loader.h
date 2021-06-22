#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "string"

class ShaderLoader
{
public:
    static GLuint LoadShader(const std::string &path);

private:
    static GLuint CompileShaderPart(GLuint shaderPartType, const std::string &path);
    static GLuint LinkShader(GLuint vertexPart, GLuint fragmentPart);
};

#endif //OPENGL_STUDY_SHADER_LOADER_H
