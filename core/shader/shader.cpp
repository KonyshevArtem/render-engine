#include "shader.h"
#include "../../math/vector4/vector4.h"
#include "../../utils/utils.h"
#include "../cubemap/cubemap.h"
#include "../texture_2d/texture_2d.h"
#include "uniform/base_uniform.h"
#include <OpenGL/gl3.h>

using namespace std;

unordered_map<string, shared_ptr<Texture>> Shader::m_GlobalTextures = {};
const Shader *                             Shader::m_CurrentShader  = nullptr;

#pragma region inner types

void Shader::BlendInfo::Apply() const
{
    if (Enabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(SrcFactor, DstFactor);
    }
    else
        glDisable(GL_BLEND);
}

#pragma endregion

#pragma region construction

shared_ptr<Shader> Shader::Load(const filesystem::path &_path, const vector<string> &_keywords)
{
    auto shader = ShaderLoader::Load(_path, _keywords);

    if (!shader)
    {
        auto fallback = ShaderLoader::Load("resources/shaders/fallback/fallback.shader", _keywords);

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

Shader::Shader(GLuint                        _program,
               unordered_map<string, string> _defaultValues,
               bool                          _zWrite,
               BlendInfo                     _blendInfo) :
    m_Program(_program),
    m_DefaultValues(std::move(_defaultValues)),
    m_ZWrite(_zWrite),
    m_BlendInfo(_blendInfo)
{
    auto lightingUniformIndex   = glGetUniformBlockIndex(m_Program, "Lighting");
    auto cameraDataUniformIndex = glGetUniformBlockIndex(m_Program, "CameraData");
    auto shadowDataUniformIndex = glGetUniformBlockIndex(m_Program, "Shadows");

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

        auto location = glGetUniformLocation(m_Program, &nameStr[0]);
        auto uniform  = make_shared<BaseUniform>(location, type, i);

        // TODO: correctly parse arrays

        if (uniform->GetType() == UniformType::UNKNOWN)
            fprintf(stderr, "Shader init error: Unknown OpenGL type for uniform %s: %d\n", &nameStr[0], type);
        else if (UniformTypeUtils::IsTexture(uniform->GetType()))
            m_TextureUnits[nameStr] = textureUnit++;

        m_Uniforms[nameStr] = std::move(uniform);
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_Program);
}

#pragma endregion

#pragma region public methods

void Shader::Use() const
{
    m_CurrentShader = this;

    glUseProgram(m_Program);
    glDepthMask(m_ZWrite ? GL_TRUE : GL_FALSE);

    m_BlendInfo.Apply();

    SetDefaultValues();
    for (const auto &pair: m_GlobalTextures)
    {
        if (pair.second != nullptr)
            SetTextureUniform(pair.first, *pair.second);
    }
}

void Shader::SetUniform(const string &_name, const void *_data) const
{
    if (this == m_CurrentShader && m_Uniforms.contains(_name) && m_Uniforms.at(_name) != nullptr)
        m_Uniforms.at(_name)->Set(_data);
}

void Shader::SetTextureUniform(const string &_name, const Texture &_texture) const
{
    if (this != m_CurrentShader || !m_TextureUnits.contains(_name))
        return;

    auto unit = m_TextureUnits.at(_name);
    _texture.Bind(unit);
    SetUniform(_name, &unit);
}

void Shader::DetachCurrentShader()
{
    if (m_CurrentShader == nullptr)
        return;

    for (const auto &pair: m_CurrentShader->m_TextureUnits)
    {
        glActiveTexture(GL_TEXTURE0 + pair.second);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindSampler(pair.second, 0);
    }

    m_CurrentShader = nullptr;
    glUseProgram(0);
    glDepthMask(GL_TRUE); // enable depth mask, otherwise depth won't be cleared
}

void Shader::SetGlobalTexture(const string &_name, shared_ptr<Texture> _texture)
{
    if (_texture == nullptr)
        return;

    m_GlobalTextures[_name] = std::move(_texture);

    if (m_CurrentShader != nullptr)
        m_CurrentShader->SetTextureUniform(_name, *m_GlobalTextures[_name]);
}

#pragma endregion

#pragma region service methods

void InitDefaultTextures(unordered_map<string, unordered_map<UniformType, shared_ptr<Texture>>> &_defaultTextures)
{
    _defaultTextures["white"][UniformType::SAMPLER_2D]   = Texture2D::White();
    _defaultTextures["white"][UniformType::SAMPLER_CUBE] = Cubemap::White();
    _defaultTextures["normal"][UniformType::SAMPLER_2D]  = Texture2D::Normal();
}

void Shader::SetDefaultValues() const
{
    static unordered_map<string, unordered_map<UniformType, shared_ptr<Texture>>> defaultTextures;
    if (defaultTextures.empty())
        InitDefaultTextures(defaultTextures);

    for (const auto &pair: m_Uniforms)
    {
        auto uniformName = pair.first;
        if (!m_DefaultValues.contains(uniformName) || pair.second == nullptr)
            continue;

        auto defaultValueLiteral = m_DefaultValues.at(uniformName);
        auto type                = pair.second->GetType();

        if (UniformTypeUtils::IsTexture(type))
        {
            if (!defaultTextures.contains(defaultValueLiteral) || !defaultTextures.at(defaultValueLiteral).contains(type))
                return;

            SetTextureUniform(uniformName, *defaultTextures.at(defaultValueLiteral).at(type));

            Vector4 st = Vector4(0, 0, 1, 1);
            SetUniform(uniformName + "ST", &st);
        }
        else
        {
            // TODO: add support for default values for other types
        }
    }
}

#pragma endregion