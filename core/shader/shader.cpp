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
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(Program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(Program, "CameraData");

    glUniformBlockBinding(Program, cameraDataUniformIndex, 0);
    glUniformBlockBinding(Program, lightingUniformIndex, 1);

    GLint count;
    GLint buffSize;
    glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffSize);

    GLsizei length;
    GLenum  type;
    GLchar  name[buffSize];
    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(Program, i, buffSize, &length, nullptr, &type, &name[0]);
        string nameStr(&name[0], length);

        UniformInfo info {};
        info.Location       = glGetUniformLocation(Program, &nameStr[0]);
        info.Type           = ConvertUniformType(type);
        m_Uniforms[nameStr] = info;

        if (info.Type == UNKNOWN)
            fprintf(stderr, "Unknown OpenGL type for uniform %s: %d\n", &nameStr[0], type);
    }
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

UniformType Shader::ConvertUniformType(GLenum _type)
{
    switch (_type)
    {
        case GL_INT:
            return INT;

        case GL_FLOAT:
            return FLOAT;
        case GL_FLOAT_VEC3:
            return FLOAT_VEC3;
        case GL_FLOAT_VEC4:
            return FLOAT_VEC4;
        case GL_FLOAT_MAT4:
            return FLOAT_MAT4;

        case GL_BOOL:
            return BOOL;

        case GL_SAMPLER_2D:
            return SAMPLER_2D;

        default:
            return UNKNOWN;
    }
}

Shader::~Shader()
{
    glDeleteProgram(Program);
}

void Shader::SetUniform(const string &_name, const void *_data)
{
    if (!m_Uniforms.contains(_name))
        return;

    UniformInfo info = m_Uniforms[_name];

    switch (info.Type)
    {
        case UNKNOWN:
            break;
        case INT: // NOLINT(bugprone-branch-clone)
        case BOOL:
        case SAMPLER_2D:
            glUniform1i(info.Location, *((GLint *) _data));
            break;
        case FLOAT:
            glUniform1f(info.Location, *((GLfloat *) _data));
            break;
        case FLOAT_VEC3:
            glUniform3fv(info.Location, 1, (GLfloat *) _data);
            break;
        case FLOAT_VEC4:
            glUniform4fv(info.Location, 1, (GLfloat *) _data);
            break;
        case FLOAT_MAT4:
            glUniformMatrix4fv(info.Location, 1, GL_FALSE, (GLfloat *) _data);
            break;
    }
}