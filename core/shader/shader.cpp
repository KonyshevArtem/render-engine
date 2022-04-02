#include "shader.h"
#include "core_debug/debug.h"
#include "cubemap/cubemap.h"
#include "property_block/property_block.h"
#include "texture_2d/texture_2d.h"
#include "uniform_info/uniform_info.h"
#include "utils.h"

typedef std::unordered_map<std::string, std::unordered_map<UniformType, std::shared_ptr<Texture>>> DefaultTexturesMap;

PropertyBlock Shader::m_PropertyBlock;
std::string   Shader::m_ReplacementTag    = "";
const Shader *Shader::m_CurrentShader     = nullptr;
const Shader *Shader::m_ReplacementShader = nullptr;

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

std::shared_ptr<Shader> Shader::Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords)
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

Shader::Shader(GLuint                                       _program,
               std::unordered_map<std::string, std::string> _defaultValues,
               std::unordered_map<std::string, std::string> _tags,
               bool                                         _zWrite,
               GLenum                                       _zTest,
               BlendInfo                                    _blendInfo) :
    m_Program(_program),
    m_DefaultValues(std::move(_defaultValues)),
    m_Tags(std::move(_tags)),
    m_ZWrite(_zWrite),
    m_ZTest(_zTest),
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

    GLsizei             length;
    GLenum              type;
    GLint               size;
    std::vector<GLchar> name(buffSize);
    int                 textureUnit = 0;

    for (int i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_Program, i, buffSize, &length, &size, &type, &name[0]);
        std::string nameStr(name.begin(), name.begin() + length);

        auto location      = glGetUniformLocation(m_Program, &nameStr[0]);
        auto convertedType = UniformUtils::ConvertUniformType(type);

        // TODO: correctly parse arrays

        if (convertedType == UniformType::UNKNOWN)
            Debug::LogErrorFormat("[Shader] Init error: Unknown OpenGL type for uniform %1%", {nameStr});
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
    glDepthFunc(m_CurrentShader->m_ZTest);

    m_CurrentShader->m_BlendInfo.Apply();

    m_CurrentShader->SetDefaultValues();

    SetPropertyBlock(m_PropertyBlock);

    return true;
}

void Shader::SetUniform(const std::string &_name, const void *_data)
{
    if (m_CurrentShader != nullptr && m_CurrentShader->m_Uniforms.contains(_name))
        UniformUtils::SetUniform(m_CurrentShader->m_Uniforms.at(_name), _data);
}

void Shader::SetTextureUniform(const std::string &_name, const Texture &_texture)
{
    if (m_CurrentShader == nullptr || !m_CurrentShader->m_TextureUnits.contains(_name))
        return;

    auto unit = m_CurrentShader->m_TextureUnits.at(_name);
    _texture.Bind(unit);
    SetUniform(_name, &unit);

    Vector4 st = Vector4(0, 0, 1, 1);
    SetUniform(_name + "_ST", &st);

    int  width     = _texture.GetWidth();
    int  height    = _texture.GetHeight();
    auto texelSize = Vector4 {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height};
    SetUniform(_name + "_TexelSize", &texelSize);
}

void Shader::SetReplacementShader(const Shader *_shader, const std::string &_tag)
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


void Shader::SetPropertyBlock(const PropertyBlock &_propertyBlock)
{
    for (const auto &pair: _propertyBlock.m_Textures)
    {
        if (pair.second != nullptr)
            SetTextureUniform(pair.first, *pair.second);
    }
    for (const auto &pair: _propertyBlock.m_Vectors)
        SetUniform(pair.first, &pair.second);
    for (const auto &pair: _propertyBlock.m_Floats)
        SetUniform(pair.first, &pair.second);
    for (const auto &pair: _propertyBlock.m_Matrices)
        SetUniform(pair.first, &pair.second);
}


void Shader::SetGlobalTexture(const std::string &_name, std::shared_ptr<Texture> _value)
{
    m_PropertyBlock.SetTexture(_name, _value);
}

const std::shared_ptr<Texture> Shader::GetGlobalTexture(const std::string &_name)
{
    return m_PropertyBlock.GetTexture(_name);
}


void Shader::SetGlobalVector(const std::string &_name, const Vector4 &_value)
{
    m_PropertyBlock.SetVector(_name, _value);
}

Vector4 Shader::GetGlobalVector(const std::string &_name)
{
    return m_PropertyBlock.GetVector(_name);
}


void Shader::SetGlobalFloat(const std::string &_name, float _value)
{
    m_PropertyBlock.SetFloat(_name, _value);
}

float Shader::GetGlobalFloat(const std::string &_name)
{
    return m_PropertyBlock.GetFloat(_name);
}


void Shader::SetGlobalMatrix(const std::string &_name, const Matrix4x4 &_value)
{
    m_PropertyBlock.SetMatrix(_name, _value);
}

Matrix4x4 Shader::GetGlobalMatrix(const std::string &_name)
{
    return m_PropertyBlock.GetMatrix(_name);
}

#pragma endregion

#pragma region service methods

DefaultTexturesMap GetDefaultTexturesMap()
{
    DefaultTexturesMap map;
    map["white"][UniformType::SAMPLER_2D]   = Texture2D::White();
    map["white"][UniformType::SAMPLER_CUBE] = Cubemap::White();
    map["normal"][UniformType::SAMPLER_2D]  = Texture2D::Normal();
    return map;
}

void Shader::SetDefaultValues() const
{
    static DefaultTexturesMap defaultTextures = GetDefaultTexturesMap();

    for (const auto &pair: m_Uniforms)
    {
        auto uniformName = pair.first;
        if (!m_DefaultValues.contains(uniformName))
            continue;

        auto defaultValueLiteral = m_DefaultValues.at(uniformName);
        auto type                = pair.second.Type;

        if (UniformUtils::IsTexture(type))
        {
            if (defaultTextures.contains(defaultValueLiteral) && defaultTextures.at(defaultValueLiteral).contains(type))
                SetTextureUniform(uniformName, *defaultTextures.at(defaultValueLiteral).at(type));
        }
        else
        {
            // TODO: add support for default values for other types
        }
    }
}

#pragma endregion