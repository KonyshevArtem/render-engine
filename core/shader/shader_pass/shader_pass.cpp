#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "types/graphics_backend_uniform_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "global_constants.h"
#include "utils.h"

#include <vector>

std::shared_ptr<Texture> GetTextureByLiteralAndType(const std::string &literal, UniformDataType dataType)
{
    if (literal == "white")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_2D:
                return Texture2D::White();
            case UniformDataType::SAMPLER_CUBE:
                return Cubemap::White();
        }
    }

    if (literal == "black")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_CUBE:
                return Cubemap::Black();
        }
    }

    if (literal == "normal")
    {
        switch (dataType)
        {
            case UniformDataType::SAMPLER_2D:
                return Texture2D::Normal();
        }
    }

    return nullptr;
}

void FillDefaultValuesPropertyBlock(const std::unordered_map<std::string, std::string> &defaultValues,
                                    const std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms,
                                    PropertyBlock &propertyBlock)
{
    for (const auto &pair: defaultValues)
    {
        auto &uniformName = pair.first;

        auto it = uniforms.find(uniformName);
        if (it == uniforms.end())
            continue;

        auto &defaultValueLiteral = pair.second;
        auto uniformDataType = it->second.Type;

        if (UniformDataTypeUtils::IsTexture(uniformDataType))
        {
            auto texture = GetTextureByLiteralAndType(defaultValueLiteral, uniformDataType);
            if (texture != nullptr)
            {
                propertyBlock.SetTexture(uniformName, texture);
            }
        }
        else
        {
            // TODO: add support for default values for other types
        }
    }
}

size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, TextureInternalFormat depthTargetFormat)
{
    size_t targetsHash = Utils::HashCombine(std::hash<TextureInternalFormat>{}(colorTargetFormat), std::hash<TextureInternalFormat>{}(depthTargetFormat));
    return Utils::HashCombine(targetsHash, vertexAttributesHash);
}

ShaderPass::ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       std::unordered_map<std::string, std::string> &tags, const std::unordered_map<std::string, std::string> &defaultValues) :
        m_Shaders(std::move(shaders)),
        m_CullInfo(cullInfo),
        m_BlendInfo(blendInfo),
        m_Tags(std::move(tags))
{
    const TextureInternalFormat k_DefaultColorFormat = TextureInternalFormat::RGBA16F;
    const TextureInternalFormat k_DefaultDepthFormat = TextureInternalFormat::DEPTH_COMPONENT;

    static std::vector<GraphicsBackendVertexAttributeDescriptor> s_DefaultVertexAttributes;
    if (s_DefaultVertexAttributes.empty())
    {
        s_DefaultVertexAttributes.push_back({0, 3, VertexAttributeDataType::FLOAT, 0, 12, 0});
    }

    CreatePSO(m_Shaders, m_BlendInfo, k_DefaultColorFormat, k_DefaultDepthFormat, s_DefaultVertexAttributes, &m_Uniforms, &m_Buffers);

    FillDefaultValuesPropertyBlock(defaultValues, m_Uniforms, m_DefaultValuesBlock);

    m_DepthStencilState = GraphicsBackend::Current()->CreateDepthStencilState(depthInfo.WriteDepth, depthInfo.DepthFunction);
}

ShaderPass::~ShaderPass()
{
    for (auto &pair : m_Programs)
    {
        GraphicsBackend::Current()->DeleteProgram(pair.second);
    }

    for (auto &shader : m_Shaders)
    {
        GraphicsBackend::Current()->DeleteShader(shader);
    }

    GraphicsBackend::Current()->DeleteDepthStencilState(m_DepthStencilState);
}

const GraphicsBackendProgram &ShaderPass::GetProgram(const VertexAttributes &vertexAttributes, TextureInternalFormat colorTargetFormat, TextureInternalFormat depthTargetFormat)
{
    if (!GraphicsBackend::Current()->RequireStrictPSODescriptor() && !m_Programs.empty())
    {
        return m_Programs.begin()->second;
    }

    auto hash = GetPSOHash(vertexAttributes.GetHash(), colorTargetFormat, depthTargetFormat);

    auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
    {
        return it->second;
    }

    return CreatePSO(m_Shaders, m_BlendInfo, colorTargetFormat, depthTargetFormat, vertexAttributes.GetAttributes(), nullptr, nullptr);
}

std::string ShaderPass::GetTagValue(const std::string &tag) const
{
    auto it = m_Tags.find(tag);
    return it != m_Tags.end() ? it->second : "";
}

const GraphicsBackendProgram &ShaderPass::CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat,
                                                    TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                                                    std::unordered_map<std::string, GraphicsBackendUniformInfo> *uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> *buffers)
{
    GraphicsBackendColorAttachmentDescriptor colorAttachmentDescriptor{};
    colorAttachmentDescriptor.Format = colorFormat;
    colorAttachmentDescriptor.SourceFactor = blendInfo.SourceFactor;
    colorAttachmentDescriptor.DestinationFactor = blendInfo.DestinationFactor;
    colorAttachmentDescriptor.BlendingEnabled = blendInfo.Enabled;

    auto program = GraphicsBackend::Current()->CreateProgram(shaders, colorAttachmentDescriptor, depthFormat, vertexAttributes, uniforms, buffers);

    size_t hash = GetPSOHash(VertexAttributes::GetHash(vertexAttributes), colorFormat, depthFormat);
    m_Programs[hash] = program;

    return m_Programs[hash];
}
