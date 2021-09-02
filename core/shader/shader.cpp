#include "shader.h"
#include "../../core/graphics/graphics.h"
#include "../../utils/utils.h"
#include "OpenGL/gl3.h"
#include "string"
#include <cstdio>
#include <cstdlib>

using namespace std;

Shader::Shader(GLuint _program)
{
    m_Program                     = _program;
    GLuint lightingUniformIndex   = glGetUniformBlockIndex(m_Program, "Lighting");
    GLuint cameraDataUniformIndex = glGetUniformBlockIndex(m_Program, "CameraData");

    glUniformBlockBinding(m_Program, cameraDataUniformIndex, 0);
    glUniformBlockBinding(m_Program, lightingUniformIndex, 1);

    GLint count;
    GLint buffSize;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffSize);

    GLsizei length;
    GLenum  type;
    GLchar  name[buffSize];
    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_Program, i, buffSize, &length, nullptr, &type, &name[0]);
        string nameStr(&name[0], length);

        UniformInfo info {};
        info.Location       = glGetUniformLocation(m_Program, &nameStr[0]);
        info.Type           = ConvertUniformType(type);
        info.Index          = i;
        m_Uniforms[nameStr] = info;

        if (info.Type == UNKNOWN)
            fprintf(stderr, "Unknown OpenGL type for uniform %s: %d\n", &nameStr[0], type);
    }
}

shared_ptr<Shader> Shader::Load(const string &_path, const vector<string> &_keywords, bool _silent)
{
    GLuint vertexPart;
    GLuint fragmentPart;
    GLuint program;

    string shaderSource = Utils::ReadFileWithIncludes(_path);

    bool success = Shader::TryCompileShaderPart(GL_VERTEX_SHADER, _path, shaderSource.c_str(), vertexPart, _keywords);
    success &= Shader::TryCompileShaderPart(GL_FRAGMENT_SHADER, _path, shaderSource.c_str(), fragmentPart, _keywords);
    success &= Shader::TryLinkProgram(vertexPart, fragmentPart, program);

    if (!success)
    {
        if (_silent)
            return GetFallbackShader();
        else
            exit(1);
    }

    return shared_ptr<Shader>(new Shader(program));
}

bool Shader::TryCompileShaderPart(GLuint                _shaderPartType,
                                  const string &        _path,
                                  const char *          _source,
                                  GLuint &              _shaderPart,
                                  const vector<string> &_keywords)
{
    const string &defines      = Graphics::GetShaderCompilationDefines();
    int           sourcesCount = 4;
    const char *  sources[sourcesCount];

    string keywordsDefines;
    for (const auto &keyword: _keywords)
        keywordsDefines += "#define " + keyword + "\n";

    sources[0] = defines.c_str();
    sources[1] = GetShaderPartDefine(_shaderPartType);
    sources[2] = keywordsDefines.c_str();
    sources[3] = _source;

    GLuint shader = glCreateShader(_shaderPartType);
    glShaderSource(shader, sourcesCount, sources, nullptr);

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
        return false;
    }

    _shaderPart = shader;
    return true;
}

const char *Shader::GetShaderPartDefine(GLuint _shaderPartType)
{
    switch (_shaderPartType)
    {
        case GL_VERTEX_SHADER:
            return "#define VERTEX\n"
                   "#define VAR out\n";
        case GL_FRAGMENT_SHADER:
            return "#define FRAGMENT\n"
                   "#define VAR in\n";
        default:
            return "";
    }
}

bool Shader::TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program)
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
        return false;
    }

    glDetachShader(program, _vertexPart);
    glDetachShader(program, _fragmentPart);

    glDeleteShader(_vertexPart);
    glDeleteShader(_fragmentPart);

    _program = program;
    return true;
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
    glDeleteProgram(m_Program);
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

shared_ptr<Shader> Shader::GetFallbackShader()
{
    if (FallbackShader == nullptr)
        FallbackShader = Shader::Load("shaders/fallback.glsl", vector<string>(), false);
    return FallbackShader;
}

shared_ptr<Shader> Shader::LoadForInit(const string &_path)
{
    GLuint vertexPart;
    GLuint program;

    string shaderSource = Utils::ReadFileWithIncludes(_path);
    shaderSource += "\nvoid main(){}\n";

    bool success = Shader::TryCompileShaderPart(GL_VERTEX_SHADER, _path, shaderSource.c_str(), vertexPart, vector<string>());
    success &= Shader::TryLinkProgram(vertexPart, 0, program);

    if (!success)
        exit(1);

    return shared_ptr<Shader>(new Shader(program));
}
