#include "shader.h"
#include "../../utils/utils.h"
#include "OpenGL/gl3.h"
#include "string"
#include <cstdio>
#include <cstdlib>

using namespace std;

Shader::Shader(GLuint _program)
{
    Program                       = _program;
    ModelMatrixLocation           = glGetUniformLocation(_program, "_ModelMatrix");
    ModelNormalMatrixLocation     = glGetUniformLocation(_program, "_ModelNormalMatrix");
    SmoothnessLocation            = glGetUniformLocation(_program, "_Smoothness");
    AlbedoLocation                = glGetUniformLocation(_program, "_Albedo");
    AlbedoSTLocation              = glGetUniformLocation(_program, "_AlbedoST");
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(_program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(_program, "CameraData");

    glUniformBlockBinding(_program, cameraDataUniformIndex, 0);
    glUniformBlockBinding(_program, lightingUniformIndex, 1);
}

shared_ptr<Shader> Shader::Load(const string &_path)
{
    GLuint vertexPart   = Shader::CompileShaderPart(GL_VERTEX_SHADER, _path + "/vert.glsl");
    GLuint fragmentPart = Shader::CompileShaderPart(GL_FRAGMENT_SHADER, _path + "/frag.glsl");

    GLuint program = Shader::LinkProgram(vertexPart, fragmentPart);
    return shared_ptr<Shader>(new Shader(program));
}

GLuint Shader::CompileShaderPart(GLuint _shaderPartType, const string &_path)
{
    string      shaderSource    = Utils::ReadFileWithIncludes(_path);
    const char *shaderSourcePtr = shaderSource.c_str();

    GLuint shader = glCreateShader(_shaderPartType);
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

        fprintf(stderr, "Shader compilation failed: %s\n%s\n", _path.c_str(), logMsg);

        free(logMsg);
    }
    else
    {
        printf("Shader compile success: %s\n", _path.c_str());
    }

    return shader;
}

GLuint Shader::LinkProgram(GLuint _vertexPart, GLuint _fragmentPart)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, _vertexPart);
    glAttachShader(program, _fragmentPart);

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

    glDetachShader(program, _vertexPart);
    glDetachShader(program, _fragmentPart);

    glDeleteShader(_vertexPart);
    glDeleteShader(_fragmentPart);

    return program;
}

Shader::~Shader()
{
    glDeleteProgram(Program);
}
