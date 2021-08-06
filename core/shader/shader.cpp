#include "shader.h"
#include "../../utils/utils.h"
#include "OpenGL/gl3.h"
#include "string"
#include <cstdio>
#include <cstdlib>

using namespace std;

Shader::Shader(GLuint program)
{
    Program                       = program;
    ModelMatrixLocation           = glGetUniformLocation(program, "_ModelMatrix");
    ModelNormalMatrixLocation     = glGetUniformLocation(program, "_ModelNormalMatrix");
    SmoothnessLocation            = glGetUniformLocation(program, "_Smoothness");
    AlbedoLocation                = glGetUniformLocation(program, "_Albedo");
    AlbedoSTLocation              = glGetUniformLocation(program, "_AlbedoST");
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(program, "CameraData");

    glUniformBlockBinding(program, cameraDataUniformIndex, 0);
    glUniformBlockBinding(program, lightingUniformIndex, 1);
}

shared_ptr<Shader> Shader::Load(const string &path)
{
    GLuint vertexPart   = Shader::CompileShaderPart(GL_VERTEX_SHADER, path + "/vert.glsl");
    GLuint fragmentPart = Shader::CompileShaderPart(GL_FRAGMENT_SHADER, path + "/frag.glsl");

    GLuint program = Shader::LinkProgram(vertexPart, fragmentPart);
    return shared_ptr<Shader>(new Shader(program));
}

GLuint Shader::CompileShaderPart(GLuint shaderPartType, const string &path)
{
    string      shaderSource    = Utils::ReadFileWithIncludes(path);
    const char *shaderSourcePtr = shaderSource.c_str();

    GLuint shader = glCreateShader(shaderPartType);
    glShaderSource(shader, 1, &shaderSourcePtr, nullptr);

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
    {
        printf("Shader compile success: %s\n", path.c_str());
    }

    return shader;
}

GLuint Shader::LinkProgram(GLuint vertexPart, GLuint fragmentPart)
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
