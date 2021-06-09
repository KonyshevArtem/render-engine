#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#define GL_SILENCE_DEPRECATION

#include "GLUT/glut.h"
#include "string"

class ShaderLoader
{
public:
    static GLuint LoadShader(GLenum shaderType, const std::string& path);
};

#endif//OPENGL_STUDY_SHADER_LOADER_H
