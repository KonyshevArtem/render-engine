#include "shader.h"
#include "../../math/vector4/vector4.h"
#include "../../utils/utils.h"
#include "../core_debug/debug.h"
#include "../cubemap/cubemap.h"
#include "../texture_2d/texture_2d.h"
#include "uniform_info/uniform_info.h"

using namespace std;

unordered_map<string, shared_ptr<Texture>> Shader::m_GlobalTextures    = {};
string                                     Shader::m_ReplacementTag    = "";
const Shader *                             Shader::m_CurrentShader     = nullptr;
const Shader *                             Shader::m_ReplacementShader = nullptr;

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
               unordered_map<string, string> _tags,
               bool                          _zWrite,
               BlendInfo                     _blendInfo) :
    m_Program(_program),
    m_DefaultValues(std::move(_defaultValues)),
    m_Tags(std::move(_tags)),
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

    GLsizei        length;
    GLenum         type;
    GLint          size;
    vector<GLchar> name(buffSize);
    int            textureUnit = 0;

    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_Program, i, buffSize, &length, &size, &type, &name[0]);
        string nameStr(name.begin(), name.begin() + length);

        auto location      = glGetUniformLocation(m_Program, &nameStr[0]);
        auto convertedType = UniformUtils::ConvertUniformType(type);

        // TODO: correctly parse arrays

        if (convertedType == UniformType::UNKNOWN)
            Debug::LogErrorFormat("[Shader] Init error: Unknown OpenGL type for uniform %1%", std::initializer_list {nameStr});
        else if (UniformUtils::IsTexture(convertedType))
            m_TextureUnits[nameStr] = textureUnit++;

        m_Uniforms[nameStr] = UniformInfo {convertedType, location, i};
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_Program);
}

#pragma endregion

#pragma region public methods

bool Shader::Use() const
{
    if (m_ReplacementShader == nullptr)
        m_CurrentShader = this;
    else if (m_ReplacementShader->m_Tags.contains(m_ReplacementTag) && m_Tags.contains(m_ReplacementTag) &&
             m_ReplacementShader->m_Tags.at(m_ReplacementTag) == m_Tags.at(m_ReplacementTag))
        m_CurrentShader = m_ReplacementShader;

    if (m_CurrentShader == nullptr)
        return false;

    glUseProgram(m_CurrentShader->m_Program);
    glDepthMask(m_CurrentShader->m_ZWrite ? GL_TRUE : GL_FALSE);

    m_CurrentShader->m_BlendInfo.Apply();

    m_CurrentShader->SetDefaultValues();
    for (const auto &pair: m_CurrentShader->m_GlobalTextures)
    {
        if (pair.second != nullptr)
            m_CurrentShader->SetTextureUniform(pair.first, *pair.second);
    }

    return true;
}

void Shader::SetUniform(const string &_name, const void *_data)
{
    if (m_CurrentShader != nullptr && m_CurrentShader->m_Uniforms.contains(_name))
        UniformUtils::SetUniform(m_CurrentShader->m_Uniforms.at(_name), _data);
}

void Shader::SetTextureUniform(const string &_name, const Texture &_texture)
{
    if (m_CurrentShader == nullptr || !m_CurrentShader->m_TextureUnits.contains(_name))
        return;

    auto unit = m_CurrentShader->m_TextureUnits.at(_name);
    _texture.Bind(unit);
    SetUniform(_name, &unit);
}

void Shader::SetReplacementShader(const Shader *_shader, const string &_tag)
{
    m_ReplacementShader = _shader;
    m_ReplacementTag    = _tag;
}

void Shader::DetachReplacementShader()
{
    if (m_CurrentShader == m_ReplacementShader)
        m_CurrentShader = nullptr;
    m_ReplacementShader = nullptr;
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
        if (!m_DefaultValues.contains(uniformName))
            continue;

        auto defaultValueLiteral = m_DefaultValues.at(uniformName);
        auto type                = pair.second.Type;

        if (UniformUtils::IsTexture(type))
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