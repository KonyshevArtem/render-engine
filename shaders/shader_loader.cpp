#include "shader_loader.h"
#include "../utils/utils.h"
#include "string"
#include <cstdio>
#include <cstdlib>

GLuint ShaderLoader::LoadShader(const std::string &path)
{
    GLuint vertexPart   = ShaderLoader::CompileShaderPart(GL_VERTEX_SHADER, path + "/vert.glsl");
    GLuint fragmentPart = ShaderLoader::CompileShaderPart(GL_FRAGMENT_SHADER, path + "/frag.glsl");
    return ShaderLoader::LinkShader(vertexPart, fragmentPart);
}

GLuint ShaderLoader::CompileShaderPart(GLuint shaderPartType, const std::string &path)
{
    char *shaderSource = Utils::ReadFile(path);

    GLuint shader = glCreateShader(shaderPartType);
    glShaderSource(shader, 1, &shaderSource, nullptr);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *logMsg = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, logMsg);

        fprintf(stderr, "Shader compilation failed: %s\n%s\n", path.c_str(), logMsg);

        free(logMsg);
    }
    else
        printf("Shader compile success: %s\n", path.c_str());

    free(shaderSource);

    return shader;
}

GLuint ShaderLoader::LinkShader(GLuint vertexPart, GLuint fragmentPart)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertexPart);
    glAttachShader(program, fragmentPart);

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

        free(logMsg);
        exit(1);
    }

    glDetachShader(program, vertexPart);
    glDetachShader(program, fragmentPart);

    return program;
}
