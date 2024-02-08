#include "shader.h"
#include "cubemap/cubemap.h"
#include "texture_2d/texture_2d.h"
#include "uniform_info/uniform_info.h"
#include "graphics_backend_api.h"
#include "enums/program_parameter.h"
#include "enums/graphics_backend_capability.h"
#include "property_block/property_block.h"
#include "enums/uniform_block_parameter.h"
#include "graphics/uniform_block.h"
#include "graphics/graphics.h"
#include "shader_pass/shader_pass.h"

typedef std::unordered_map<std::string, std::unordered_map<UniformDataType, std::shared_ptr<Texture>>> DefaultTexturesMap;

std::shared_ptr<ShaderPass> m_CurrentPass;

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

Shader::Shader(std::vector<std::shared_ptr<ShaderPass>> _passes, std::unordered_map<std::string, std::string> _defaultValues, bool _supportInstancing) :
    m_Passes(std::move(_passes)),
    m_DefaultValues(std::move(_defaultValues)),
    m_SupportInstancing(_supportInstancing)
{
}

#pragma endregion

#pragma region public methods

void Shader::Use(int _passIndex) const
{
    static const Shader* shaderInUse = nullptr;
    static int passIndexInUse = -1;

    if (_passIndex < 0 || _passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    m_CurrentPass = m_Passes.at(_passIndex);

    if (shaderInUse != this || _passIndex != passIndexInUse)
    {
        shaderInUse = this;
        passIndexInUse = _passIndex;

        GraphicsBackend::UseProgram(m_CurrentPass->GetProgram());

        auto &blendInfo = m_CurrentPass->GetBlendInfo();
        auto &cullInfo = m_CurrentPass->GetCullInfo();
        auto &depthInfo = m_CurrentPass->GetDepthInfo();
        Graphics::SetBlendState(blendInfo.Enabled, blendInfo.SourceFactor, blendInfo.DestinationFactor);
        Graphics::SetCullState(cullInfo.Enabled, cullInfo.Face);
        Graphics::SetDepthState(depthInfo.WriteDepth, depthInfo.DepthFunction);
        SetDefaultValues(m_CurrentPass->GetUniforms());
    }
}

std::string Shader::GetPassTagValue(int _passIndex, const std::string &_tag) const
{
    if (_passIndex < 0 || _passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    return m_Passes.at(_passIndex)->GetTagValue(_tag);
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

void Shader::SetUniformBlock(const UniformBlock &uniformBlock)
{
    if (m_CurrentPass == nullptr)
    {
        return;
    }

    auto it = m_CurrentPass->GetUniformBlockBindings().find(uniformBlock.GetName());
    if (it != m_CurrentPass->GetUniformBlockBindings().end())
    {
        uniformBlock.Bind(it->second);
    }
}

#pragma endregion

#pragma region service methods

DefaultTexturesMap GetDefaultTexturesMap()
{
    DefaultTexturesMap map;
    map["white"][UniformDataType::SAMPLER_2D]   = Texture2D::White();
    map["white"][UniformDataType::SAMPLER_CUBE] = Cubemap::White();
    map["black"][UniformDataType::SAMPLER_CUBE] = Cubemap::Black();
    map["normal"][UniformDataType::SAMPLER_2D]  = Texture2D::Normal();
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
        auto type = pair.second.Type;

        if (UniformDataTypeUtils::IsTexture(type))
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
    if (m_CurrentPass != nullptr && m_CurrentPass->GetUniforms().contains(_name))
    {
        auto uniformInfo = m_CurrentPass->GetUniforms().at(_name);
        GraphicsBackend::SetUniform(uniformInfo.Location, uniformInfo.Type, 1, _data);
    }
}

void Shader::SetTextureUniform(const std::string &_name, const Texture &_texture)
{
    if (m_CurrentPass == nullptr || !m_CurrentPass->GetTextureUnits().contains(_name))
        return;

    auto unit = m_CurrentPass->GetTextureUnits().at(_name);
    auto unitIndex = TextureUnitUtils::TextureUnitToIndex(unit);
    _texture.Bind(unit);
    SetUniform(_name, &unitIndex);

    Vector4 st = Vector4(0, 0, 1, 1);
    SetUniform(_name + "_ST", &st);

    int  width     = _texture.GetWidth();
    int  height    = _texture.GetHeight();
    auto texelSize = Vector4 {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height};
    SetUniform(_name + "_TexelSize", &texelSize);
}

#pragma endregion