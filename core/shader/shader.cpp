#include "shader.h"
#include "../../core/graphics/graphics.h"
#include "../../math/vector4/vector4.h"
#include "../../utils/utils.h"
#include "../texture_2d/texture_2d.h"
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
    GLuint shadowDataUniformIndex = glGetUniformBlockIndex(m_Program, "Shadows");

    if (cameraDataUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, cameraDataUniformIndex, 0);

    if (lightingUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, lightingUniformIndex, 1);

    if (shadowDataUniformIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_Program, shadowDataUniformIndex, 2);

    GLint count;
    GLint buffSize;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffSize);

    GLsizei length;
    GLenum  type;
    GLchar  name[buffSize];
    int     textureUnit = 0;
    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_Program, i, buffSize, &length, nullptr, &type, &name[0]);
        string nameStr(&name[0], length);

        UniformInfo info {};
        info.Location       = glGetUniformLocation(m_Program, &nameStr[0]);
        info.Type           = ConvertUniformType(type);
        info.Index          = i;
        m_Uniforms[nameStr] = info;

        // TODO: correctly parse arrays

        if (info.Type == UniformType::UNKNOWN)
            fprintf(stderr, "Shader init error: Unknown OpenGL type for uniform %s: %d\n", &nameStr[0], type);
        else if (UniformTypeUtils::IsTexture(info.Type))
            m_TextureUnits[nameStr] = textureUnit++;
    }
}

shared_ptr<Shader> Shader::Load(const filesystem::path &_path, const vector<string> &_keywords, bool _silent)
{
    GLuint vertexPart;
    GLuint fragmentPart;
    GLuint program;

    string shaderSource = Utils::ReadFileWithIncludes(_path);

    bool success = Shader::TryCompileShaderPart(GL_VERTEX_SHADER, _path, shaderSource.c_str(), vertexPart, _keywords);
    success &= Shader::TryCompileShaderPart(GL_FRAGMENT_SHADER, _path, shaderSource.c_str(), fragmentPart, _keywords);
    success &= Shader::TryLinkProgram(vertexPart, fragmentPart, program, _path);

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
                                  const string         &_path,
                                  const char           *_source,
                                  GLuint               &_shaderPart,
                                  const vector<string> &_keywords)
{
    const string &defines      = Graphics::GetShaderCompilationDefines();
    int           sourcesCount = 4;
    const char   *sources[sourcesCount];

    string keywordsDefines;
    for (const auto &keyword: _keywords)
        keywordsDefines += "#define " + keyword + "\n";

    sources[0] = defines.c_str();
    sources[1] = keywordsDefines.c_str();
    sources[2] = _source;
    sources[3] = GetShaderPartDefine(_shaderPartType);

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
            return "\nout Varyings vars;\nvoid main(){vars=VERTEX();gl_Position=vars.PositionCS;}\n";
        case GL_FRAGMENT_SHADER:
            return "\nin Varyings vars;\nout vec4 outColor;\nvoid main(){outColor=FRAGMENT(vars);}\n";
        default:
            return "";
    }
}

bool Shader::TryLinkProgram(GLuint _vertexPart, GLuint _fragmentPart, GLuint &_program, const string &_path)
{
    GLuint program = glCreateProgram();

    bool hasVertex   = glIsShader(_vertexPart);
    bool hasFragment = glIsShader(_fragmentPart);

    if (hasVertex)
        glAttachShader(program, _vertexPart);
    if (hasFragment)
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
        fprintf(stderr, "Program link failed: %s\n%s", _path.c_str(), logMsg);

        free(logMsg);
        return false;
    }

    if (hasVertex)
    {
        glDetachShader(program, _vertexPart);
        glDeleteShader(_vertexPart);
    }

    if (hasFragment)
    {
        glDetachShader(program, _fragmentPart);
        glDeleteShader(_fragmentPart);
    }

    _program = program;
    return true;
}

UniformType Shader::ConvertUniformType(GLenum _type)
{
    switch (_type)
    {
        case GL_INT:
            return UniformType::INT;

        case GL_FLOAT:
            return UniformType::FLOAT;
        case GL_FLOAT_VEC3:
            return UniformType::FLOAT_VEC3;
        case GL_FLOAT_VEC4:
            return UniformType::FLOAT_VEC4;
        case GL_FLOAT_MAT4:
            return UniformType::FLOAT_MAT4;

        case GL_BOOL:
            return UniformType::BOOL;

        case GL_SAMPLER_2D:
            return UniformType::SAMPLER_2D;
        case GL_SAMPLER_2D_ARRAY:
            return UniformType::SAMPLER_2D_ARRAY;
        case GL_SAMPLER_CUBE:
            return UniformType::SAMPLER_CUBE;

        default:
            return UniformType::UNKNOWN;
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_Program);
}

void Shader::Use() const
{
    glUseProgram(m_Program);
}

const unordered_map<string, int> &Shader::GetTextureUnits() const
{
    return m_TextureUnits;
}

void Shader::SetUniform(const string &_name, const void *_data) const
{
    if (!m_Uniforms.contains(_name))
        return;

    UniformInfo info = m_Uniforms.at(_name);

    switch (info.Type)
    {
        case UniformType::UNKNOWN:
            break;
        case UniformType::INT: // NOLINT(bugprone-branch-clone)
        case UniformType::BOOL:
        case UniformType::SAMPLER_2D:
        case UniformType::SAMPLER_2D_ARRAY:
        case UniformType::SAMPLER_CUBE:
            glUniform1i(info.Location, *((GLint *) _data));
            break;
        case UniformType::FLOAT:
            glUniform1f(info.Location, *((GLfloat *) _data));
            break;
        case UniformType::FLOAT_VEC3:
            glUniform3fv(info.Location, 1, (GLfloat *) _data);
            break;
        case UniformType::FLOAT_VEC4:
            glUniform4fv(info.Location, 1, (GLfloat *) _data);
            break;
        case UniformType::FLOAT_MAT4:
            glUniformMatrix4fv(info.Location, 1, GL_FALSE, (GLfloat *) _data);
            break;
    }
}

void Shader::BindDefaultTextures() const
{
    auto white  = Texture2D::White();
    auto normal = Texture2D::Normal();
    auto units  = GetTextureUnits();

    for (const auto &pair: m_Uniforms)
    {
        if (!UniformTypeUtils::IsTexture(pair.second.Type) || !units.contains(pair.first))
            continue;

        int unit = units.at(pair.first);
        if (pair.second.Type == UniformType::SAMPLER_2D)
        {
            if (pair.first == "_NormalMap") // TODO: rework
                normal->Bind(unit);
            else
                white->Bind(unit);

            Vector4 st = Vector4(0, 0, 1, 1);
            SetUniform(pair.first, &unit);
            SetUniform(pair.first + "ST", &st);
        }
        else if (pair.second.Type == UniformType::SAMPLER_2D_ARRAY)
        {
            // TODO: bind default texture array
        }
    }
}

const shared_ptr<Shader> &Shader::GetFallbackShader()
{
    if (FallbackShader == nullptr)
        FallbackShader = Shader::Load("resources/shaders/fallback.glsl", vector<string>(), false);
    return FallbackShader;
}
