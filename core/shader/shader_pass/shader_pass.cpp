#include "shader_pass.h"
#include "graphics_backend_api.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "utils/utils.h"

#include <vector>

size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat)
{
    size_t targetsHash = Utils::HashCombine(std::hash<TextureInternalFormat>{}(colorTargetFormat), std::hash<TextureInternalFormat>{}(depthTargetFormat));
    targetsHash = Utils::HashCombine(targetsHash, std::hash<bool>{}(isLinear));
    return Utils::HashCombine(targetsHash, vertexAttributesHash);
}

ShaderPass::ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
                       std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
                       std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
                       std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers) :
        m_Shaders(std::move(shaders)),
        m_CullInfo(cullInfo),
        m_BlendInfo(blendInfo),
        m_Textures(std::move(textures)),
        m_Buffers(std::move(buffers)),
        m_Samplers(std::move(samplers))
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

    CreatePSO(m_Shaders, m_BlendInfo, k_DefaultColorFormat, true, k_DefaultDepthFormat, s_DefaultVertexAttributes);

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

const GraphicsBackendProgram &ShaderPass::GetProgram(const VertexAttributes &vertexAttributes, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat)
{
    if (!GraphicsBackend::Current()->RequireStrictPSODescriptor() && !m_Programs.empty())
    {
        return m_Programs.begin()->second;
    }

    auto hash = GetPSOHash(vertexAttributes.GetHash(), colorTargetFormat, isLinear, depthTargetFormat);

    auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
    {
        return it->second;
    }

    return CreatePSO(m_Shaders, m_BlendInfo, colorTargetFormat, isLinear, depthTargetFormat, vertexAttributes.GetAttributes());
}

const GraphicsBackendProgram &ShaderPass::CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat, bool isLinear,
                                                    TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes)
{
    GraphicsBackendColorAttachmentDescriptor colorAttachmentDescriptor{};
    colorAttachmentDescriptor.Format = colorFormat;
    colorAttachmentDescriptor.SourceFactor = blendInfo.SourceFactor;
    colorAttachmentDescriptor.DestinationFactor = blendInfo.DestinationFactor;
    colorAttachmentDescriptor.BlendingEnabled = blendInfo.Enabled;
    colorAttachmentDescriptor.IsLinear = isLinear;

    auto program = GraphicsBackend::Current()->CreateProgram(shaders, colorAttachmentDescriptor, depthFormat, vertexAttributes);

    size_t hash = GetPSOHash(VertexAttributes::GetHash(vertexAttributes), colorFormat, isLinear, depthFormat);
    m_Programs[hash] = program;

    return m_Programs[hash];
}
