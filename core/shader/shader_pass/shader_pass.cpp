#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "texture_2d/texture_2d.h"
#include "cubemap/cubemap.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "utils.h"

#include <vector>

std::shared_ptr<Texture> GetTextureByLiteralAndType(const std::string &literal, TextureDataType dataType)
{
    if (literal == "white")
    {
        switch (dataType)
        {
            case TextureDataType::SAMPLER_2D:
                return Texture2D::White();
            case TextureDataType::SAMPLER_CUBE:
                return Cubemap::White();
        }
    }

    if (literal == "black")
    {
        switch (dataType)
        {
            case TextureDataType::SAMPLER_CUBE:
                return Cubemap::Black();
        }
    }

    if (literal == "normal")
    {
        switch (dataType)
        {
            case TextureDataType::SAMPLER_2D:
                return Texture2D::Normal();
        }
    }

    return nullptr;
}

void FillDefaultTextures(const std::unordered_map<std::string, std::string> &defaultValues,
                         const std::unordered_map<std::string, GraphicsBackendTextureInfo> &shaderTextures,
                         std::unordered_map<std::string, std::shared_ptr<Texture>> &defaultTextures)
{
    for (const auto &pair: defaultValues)
    {
        auto &textureName = pair.first;

        auto it = shaderTextures.find(textureName);
        if (it == shaderTextures.end())
            continue;

        auto &defaultValueLiteral = pair.second;
        auto textureDataType = it->second.Type;

        auto texture = GetTextureByLiteralAndType(defaultValueLiteral, textureDataType);
        if (texture != nullptr)
        {
            defaultTextures[textureName] = texture;
        }
    }
}

size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, TextureInternalFormat depthTargetFormat)
{
    size_t targetsHash = Utils::HashCombine(std::hash<TextureInternalFormat>{}(colorTargetFormat), std::hash<TextureInternalFormat>{}(depthTargetFormat));
    return Utils::HashCombine(targetsHash, vertexAttributesHash);
}

ShaderPass::ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       const std::unordered_map<std::string, std::string> &defaultValues) :
        m_Shaders(std::move(shaders)),
        m_CullInfo(cullInfo),
        m_BlendInfo(blendInfo)
{
    const TextureInternalFormat k_DefaultColorFormat = TextureInternalFormat::RGBA16F;
    const TextureInternalFormat k_DefaultDepthFormat = TextureInternalFormat::DEPTH_COMPONENT;

    static std::vector<GraphicsBackendVertexAttributeDescriptor> s_DefaultVertexAttributes;
    if (s_DefaultVertexAttributes.empty())
    {
        s_DefaultVertexAttributes.push_back({0, 3, VertexAttributeDataType::FLOAT, 0, 44, 0});
        s_DefaultVertexAttributes.push_back({1, 3, VertexAttributeDataType::FLOAT, 0, 44, 12});
        s_DefaultVertexAttributes.push_back({2, 2, VertexAttributeDataType::FLOAT, 0, 44, 24});
        s_DefaultVertexAttributes.push_back({3, 3, VertexAttributeDataType::FLOAT, 0, 44, 32});
    }

    CreatePSO(m_Shaders, m_BlendInfo, k_DefaultColorFormat, k_DefaultDepthFormat, s_DefaultVertexAttributes, &m_Textures, &m_Buffers);

    FillDefaultTextures(defaultValues, m_Textures, m_DefaultTextures);

    m_DepthStencilState = GraphicsBackend::Current()->CreateDepthStencilState(depthInfo.WriteDepth, depthInfo.DepthFunction);
}

ShaderPass::ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       const std::unordered_map<std::string, std::string> &defaultValues,
                       std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
                       std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
                       std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers)
                       : ShaderPass(shaders, blendInfo, cullInfo, depthInfo, defaultValues)
{
    m_Textures = std::move(textures);
    m_Buffers = std::move(buffers);
    m_Samplers = std::move(samplers);
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

const GraphicsBackendProgram &ShaderPass::CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat,
                                                    TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                                                    std::unordered_map<std::string, GraphicsBackendTextureInfo> *textures, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> *buffers)
{
    GraphicsBackendColorAttachmentDescriptor colorAttachmentDescriptor{};
    colorAttachmentDescriptor.Format = colorFormat;
    colorAttachmentDescriptor.SourceFactor = blendInfo.SourceFactor;
    colorAttachmentDescriptor.DestinationFactor = blendInfo.DestinationFactor;
    colorAttachmentDescriptor.BlendingEnabled = blendInfo.Enabled;

    auto program = GraphicsBackend::Current()->CreateProgram(shaders, colorAttachmentDescriptor, depthFormat, vertexAttributes, textures, buffers);

    size_t hash = GetPSOHash(VertexAttributes::GetHash(vertexAttributes), colorFormat, depthFormat);
    m_Programs[hash] = program;

    return m_Programs[hash];
}
