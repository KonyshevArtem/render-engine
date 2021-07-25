#include "shader.h"
#include "../../utils/utils.h"
#include "OpenGL/gl3.h"
#include "string"
#include <cstdio>
#include <cstdlib>

Shader::Shader(GLuint program)
{
    Program                       = program;
    ModelMatrixLocation           = glGetUniformLocation(program, "modelMatrix");
    ModelNormalMatrixLocation     = glGetUniformLocation(program, "modelNormalMatrix");
    SmoothnessLocation            = glGetUniformLocation(program, "smoothness");
    AlbedoLocation                = glGetUniformLocation(program, "albedo");
    GLuint matricesUniformIndex   = glGetUniformBlockIndex(program, "Matrices");
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(program, "CameraData");

    glUniformBlockBinding(program, matricesUniformIndex, 0);
    glUniformBlockBinding(program, lightingUniformIndex, 1);
    glUniformBlockBinding(program, cameraDataUniformIndex, 2);
}

Shader *Shader::Load(const std::string &path)
{
    GLuint vertexPart   = Shader::CompileShaderPart(GL_VERTEX_SHADER, path + "/vert.glsl");
    GLuint fragmentPart = Shader::CompileShaderPart(GL_FRAGMENT_SHADER, path + "/frag.glsl");

    GLuint program = Shader::LinkProgram(vertexPart, fragmentPart);
    return new Shader(program);
}

GLuint Shader::CompileShaderPart(GLuint shaderPartType, const std::string &path)
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
