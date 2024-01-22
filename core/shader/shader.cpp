#include "shader.h"
#include "cubemap/cubemap.h"
#include "texture_2d/texture_2d.h"
#include "uniform_info/uniform_info.h"
#include "utils.h"

typedef std::unordered_map<std::string, std::unordered_map<UniformDataType, std::shared_ptr<Texture>>> DefaultTexturesMap;

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
        int lightingUniformIndex;
        int cameraDataUniformIndex;
        int shadowDataUniformIndex;

        if (GraphicsBackend::TryGetUniformBlockIndex(passInfo.Program, "CameraData", &cameraDataUniformIndex))
            GraphicsBackend::SetUniformBlockBinding(passInfo.Program, cameraDataUniformIndex, 0);

        if (GraphicsBackend::TryGetUniformBlockIndex(passInfo.Program, "Lighting", &lightingUniformIndex))
            GraphicsBackend::SetUniformBlockBinding(passInfo.Program, lightingUniformIndex, 1);

        if (GraphicsBackend::TryGetUniformBlockIndex(passInfo.Program, "Shadows", &shadowDataUniformIndex))
            GraphicsBackend::SetUniformBlockBinding(passInfo.Program, shadowDataUniformIndex, 2);

        int uniformCount;
        int uniformNameBufferSize;
        GraphicsBackend::GetProgramParameter(passInfo.Program, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);
        GraphicsBackend::GetProgramParameter(passInfo.Program, ProgramParameter::ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameBufferSize);

        int uniformSize;
        int uniformNameLength;
        UniformDataType uniformDataType;
        std::vector<char> uniformNameBuffer(uniformNameBufferSize);
        TextureUnit textureUnit = TextureUnit::TEXTURE0;

        for (int i = 0; i < uniformCount; ++i)
        {
            GraphicsBackend::GetActiveUniform(passInfo.Program, i, uniformNameBufferSize, &uniformNameLength, &uniformSize, &uniformDataType, &uniformNameBuffer[0]);
            std::string uniformName(uniformNameBuffer.begin(), uniformNameBuffer.begin() + uniformNameLength);

            auto location = GraphicsBackend::GetUniformLocation(passInfo.Program, &uniformName[0]);

            // TODO: correctly parse arrays

            if (UniformDataTypeUtils::IsTexture(uniformDataType))
            {
                passInfo.TextureUnits[uniformName] = textureUnit;
                textureUnit = static_cast<TextureUnit>(static_cast<int>(textureUnit) + 1);
            }

            passInfo.Uniforms[uniformName] = UniformInfo {uniformDataType, location, i};
        }
    }
}

Shader::~Shader()
{
    for (const auto &passInfo: m_Passes)
        GraphicsBackend::DeleteProgram(passInfo.Program);
}

#pragma endregion

#pragma region public methods

void Shader::Use(int _passIndex) const
{
    static const Shader* shaderInUse = nullptr;
    static int passIndexInUse = -1;

    if (_passIndex < 0 || _passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    m_CurrentPass = &m_Passes.at(_passIndex);

    if (shaderInUse != this || _passIndex != passIndexInUse)
    {
        shaderInUse = this;
        passIndexInUse = _passIndex;

        GraphicsBackend::UseProgram(m_CurrentPass->Program);

        SetBlendInfo(m_CurrentPass->BlendInfo);
        SetCullInfo(m_CurrentPass->CullInfo);
        SetDepthInfo(m_CurrentPass->DepthInfo);
        SetDefaultValues(m_CurrentPass->Uniforms);
    }

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
        GraphicsBackend::SetCapability(GraphicsBackendCapability::BLEND, true);
        GraphicsBackend::SetBlendFunction(_blendInfo.SourceFactor, _blendInfo.DestinationFactor);
    }
    else
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::BLEND, false);
    }
}

void Shader::SetCullInfo(const Shader::CullInfo &_cullInfo) const
{
    if (_cullInfo.Enabled)
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::CULL_FACE, true);
        GraphicsBackend::SetCullFace(_cullInfo.Face);
    }
    else
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::CULL_FACE, false);
    }
}

void Shader::SetDepthInfo(const Shader::DepthInfo &_depthInfo) const
{
    GraphicsBackend::SetDepthWrite(_depthInfo.WriteDepth);
    GraphicsBackend::SetDepthFunction(_depthInfo.DepthFunction);
}

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
    if (m_CurrentPass != nullptr && m_CurrentPass->Uniforms.contains(_name))
    {
        auto uniformInfo = m_CurrentPass->Uniforms.at(_name);
        GraphicsBackend::SetUniform(uniformInfo.Location, uniformInfo.Type, 1, _data);
    }
}

void Shader::SetTextureUniform(const std::string &_name, const Texture &_texture)
{
    if (m_CurrentPass == nullptr || !m_CurrentPass->TextureUnits.contains(_name))
        return;

    auto unit = m_CurrentPass->TextureUnits.at(_name);
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