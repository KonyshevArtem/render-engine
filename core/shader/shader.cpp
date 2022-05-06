#include "shader.h"
#include "core_debug/debug.h"
#include "cubemap/cubemap.h"
#include "property_block/property_block.h"
#include "texture_2d/texture_2d.h"
#include "uniform_info/uniform_info.h"
#include "utils.h"

typedef std::unordered_map<std::string, std::unordered_map<UniformType, std::shared_ptr<Texture>>> DefaultTexturesMap;

PropertyBlock           Shader::m_PropertyBlock;
const Shader::PassInfo *Shader::m_CurrentPass = nullptr;


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

Shader::Shader(std::vector<PassInfo> _passes, std::unordered_map<std::string, std::string> _defaultValues, bool _supportInstancing) :
    m_Passes(std::move(_passes)),
    m_DefaultValues(std::move(_defaultValues)),
    m_SupportInstancing(_supportInstancing)
{
    for (auto &passInfo: m_Passes)
    {
        auto lightingUniformIndex   = glGetUniformBlockIndex(passInfo.Program, "Lighting");
        auto cameraDataUniformIndex = glGetUniformBlockIndex(passInfo.Program, "CameraData");
        auto shadowDataUniformIndex = glGetUniformBlockIndex(passInfo.Program, "Shadows");

        if (cameraDataUniformIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(passInfo.Program, cameraDataUniformIndex, 0);

        if (lightingUniformIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(passInfo.Program, lightingUniformIndex, 1);

        if (shadowDataUniformIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(passInfo.Program, shadowDataUniformIndex, 2);

        GLint count;
        GLint buffSize;
        glGetProgramiv(passInfo.Program, GL_ACTIVE_UNIFORMS, &count);
        glGetProgramiv(passInfo.Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffSize);

        GLsizei             length;
        GLenum              type;
        GLint               size;
        std::vector<GLchar> name(buffSize);
        int                 textureUnit = 0;

        for (int i = 0; i < count; ++i)
        {
            glGetActiveUniform(passInfo.Program, i, buffSize, &length, &size, &type, &name[0]);
            std::string nameStr(name.begin(), name.begin() + length);

            auto location      = glGetUniformLocation(passInfo.Program, &nameStr[0]);
            auto convertedType = UniformUtils::ConvertUniformType(type);

            // TODO: correctly parse arrays

            if (convertedType == UniformType::UNKNOWN)
                Debug::LogErrorFormat("[Shader] Init error: Unknown OpenGL type for uniform %1%", {nameStr});
            else if (UniformUtils::IsTexture(convertedType))
                passInfo.TextureUnits[nameStr] = textureUnit++;

            passInfo.Uniforms[nameStr] = UniformInfo {convertedType, location, i};
        }
    }
}

Shader::~Shader()
{
    for (const auto &passInfo: m_Passes)
        glDeleteProgram(passInfo.Program);
}

#pragma endregion

#pragma region public methods

void Shader::Use(int _passIndex) const
{
    if (_passIndex < 0 || _passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    m_CurrentPass = &m_Passes.at(_passIndex);

    glUseProgram(m_CurrentPass->Program);

    SetBlendInfo(m_CurrentPass->BlendInfo);
    SetCullInfo(m_CurrentPass->CullInfo);
    SetDepthInfo(m_CurrentPass->DepthInfo);
    SetDefaultValues(m_CurrentPass->Uniforms);

    SetPropertyBlock(m_PropertyBlock);
}

std::string Shader::GetPassTagValue(int _passIndex, const std::string &_tag) const
{
    if (_passIndex < 0 || _passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    const auto &pass = m_Passes.at(_passIndex);
    return pass.Tags.contains(_tag) ? pass.Tags.at(_tag) : "";
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

void Shader::SetBlendInfo(const Shader::BlendInfo &_blendInfo) const
{
    if (_blendInfo.Enabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(_blendInfo.SrcFactor, _blendInfo.DstFactor);
    }
    else
        glDisable(GL_BLEND);
}

void Shader::SetCullInfo(const Shader::CullInfo &_cullInfo) const
{
    if (_cullInfo.Enabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(_cullInfo.Face);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Shader::SetDepthInfo(const Shader::DepthInfo &_depthInfo) const
{
    glDepthMask(_depthInfo.ZWrite ? GL_TRUE : GL_FALSE);
    glDepthFunc(_depthInfo.ZTest);
}

DefaultTexturesMap GetDefaultTexturesMap()
{
    DefaultTexturesMap map;
    map["white"][UniformType::SAMPLER_2D]   = Texture2D::White();
    map["white"][UniformType::SAMPLER_CUBE] = Cubemap::White();
    map["black"][UniformType::SAMPLER_CUBE] = Cubemap::Black();
    map["normal"][UniformType::SAMPLER_2D]  = Texture2D::Normal();
    return map;
}

void Shader::SetDefaultValues(const std::unordered_map<std::string, UniformInfo> &_uniforms) const
{
    static DefaultTexturesMap defaultTextures = GetDefaultTexturesMap();

    for (const auto &pair: _uniforms)
    {
        auto &uniformName = pair.first;
        if (!m_DefaultValues.contains(uniformName))
            continue;

        auto &defaultValueLiteral = m_DefaultValues.at(uniformName);
        auto  type                = pair.second.Type;

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

void Shader::SetUniform(const std::string &_name, const void *_data)
{
    if (m_CurrentPass != nullptr && m_CurrentPass->Uniforms.contains(_name))
        UniformUtils::SetUniform(m_CurrentPass->Uniforms.at(_name), _data);
}

void Shader::SetTextureUniform(const std::string &_name, const Texture &_texture)
{
    if (m_CurrentPass == nullptr || !m_CurrentPass->TextureUnits.contains(_name))
        return;

    auto unit = m_CurrentPass->TextureUnits.at(_name);
    _texture.Bind(unit);
    SetUniform(_name, &unit);

    Vector4 st = Vector4(0, 0, 1, 1);
    SetUniform(_name + "_ST", &st);

    int  width     = _texture.GetWidth();
    int  height    = _texture.GetHeight();
    auto texelSize = Vector4 {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height};
    SetUniform(_name + "_TexelSize", &texelSize);
}

#pragma endregion