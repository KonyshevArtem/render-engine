#include "shader.h"
#include "shader_loader/shader_loader.h"
#include "graphics_backend_api.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_program_descriptor.h"
#include "hash.h"

#include <vector>

namespace ShaderLocal
{
    size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat, PrimitiveType primitiveType)
    {
        size_t targetsHash = Hash::Combine(std::hash<TextureInternalFormat>{}(colorTargetFormat), std::hash<TextureInternalFormat>{}(depthTargetFormat));
        targetsHash = Hash::Combine(targetsHash, std::hash<bool>{}(isLinear));
        targetsHash = Hash::Combine(targetsHash, std::hash<PrimitiveType>{}(primitiveType));
        return Hash::Combine(targetsHash, vertexAttributesHash);
    }
}

std::shared_ptr<Shader> Shader::Load(const std::filesystem::path &path, const std::initializer_list<std::string> &keywords,
    BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo)
{
    auto shader = ShaderLoader::Load(path, keywords, blendInfo, cullInfo, depthInfo);

    if (!shader)
    {
        static std::shared_ptr<Shader> fallback = ShaderLoader::Load("core_resources/shaders/fallback", {}, {}, {}, {});

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

Shader::Shader(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
               std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
               std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
               std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
               std::string name, bool supportInstancing) :
    m_Shaders(std::move(shaders)),
    m_CullInfo(cullInfo),
    m_BlendInfo(blendInfo),
    m_DepthInfo(depthInfo),
    m_Textures(std::move(textures)),
    m_Buffers(std::move(buffers)),
    m_Samplers(std::move(samplers)),
    m_Name(std::move(name)),
    m_SupportInstancing(supportInstancing)
{
    const TextureInternalFormat k_DefaultColorFormat = TextureInternalFormat::RGBA16F;
    const TextureInternalFormat k_DefaultDepthFormat = TextureInternalFormat::DEPTH_32_STENCIL_8;
    const PrimitiveType k_DefaultPrimitiveType = PrimitiveType::TRIANGLES;

    static std::vector<GraphicsBackendVertexAttributeDescriptor> s_DefaultVertexAttributes;
    if (s_DefaultVertexAttributes.empty())
    {
        s_DefaultVertexAttributes.push_back({VertexAttributeSemantic::POSITION, 0, 3, VertexAttributeDataType::FLOAT, 0, 44, 0});
        s_DefaultVertexAttributes.push_back({VertexAttributeSemantic::NORMAL, 1, 3, VertexAttributeDataType::FLOAT, 0, 44, 12});
        s_DefaultVertexAttributes.push_back({VertexAttributeSemantic::TEXCOORD, 2, 2, VertexAttributeDataType::FLOAT, 0, 44, 24});
        s_DefaultVertexAttributes.push_back({VertexAttributeSemantic::TANGENT, 3, 3, VertexAttributeDataType::FLOAT, 0, 44, 32});
    }

    CreatePSO(m_Shaders, m_BlendInfo, k_DefaultColorFormat, true, k_DefaultDepthFormat, s_DefaultVertexAttributes, k_DefaultPrimitiveType, m_Name);
}

Shader::~Shader()
{
    for (auto &pair : m_Programs)
    {
        GraphicsBackend::Current()->DeleteProgram(pair.second);
    }

    for (auto &shader : m_Shaders)
    {
        GraphicsBackend::Current()->DeleteShader(shader);
    }
}

const GraphicsBackendProgram& Shader::GetProgram(const VertexAttributes &vertexAttributes, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat, PrimitiveType primitiveType)
{
    if (!GraphicsBackend::Current()->RequireStrictPSODescriptor() && !m_Programs.empty())
    {
        return m_Programs.begin()->second;
    }

    auto hash = ShaderLocal::GetPSOHash(vertexAttributes.GetHash(), colorTargetFormat, isLinear, depthTargetFormat, primitiveType);

    auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
    {
        return it->second;
    }

    return CreatePSO(m_Shaders, m_BlendInfo, colorTargetFormat, isLinear, depthTargetFormat, vertexAttributes.GetAttributes(), primitiveType, m_Name);
}

const GraphicsBackendProgram& Shader::CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat, bool isLinear,
                                                    TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, PrimitiveType primitiveType, const std::string& name)
{
    GraphicsBackendColorAttachmentDescriptor colorAttachmentDescriptor{};
    colorAttachmentDescriptor.Format = colorFormat;
    colorAttachmentDescriptor.SourceFactor = blendInfo.SourceFactor;
    colorAttachmentDescriptor.DestinationFactor = blendInfo.DestinationFactor;
    colorAttachmentDescriptor.BlendingEnabled = blendInfo.Enabled;
    colorAttachmentDescriptor.IsLinear = isLinear;

    GraphicsBackendProgramDescriptor programDescriptor{};
    programDescriptor.Shaders = &shaders;
    programDescriptor.VertexAttributes = &vertexAttributes;
    programDescriptor.Textures = &m_Textures;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Name = &name;
    programDescriptor.ColorAttachmentDescriptor = colorAttachmentDescriptor;
    programDescriptor.DepthFormat = depthFormat;
    programDescriptor.CullFace = m_CullInfo.Face;
    programDescriptor.CullFaceOrientation = m_CullInfo.Orientation;
    programDescriptor.DepthWrite = m_DepthInfo.WriteDepth;
    programDescriptor.DepthFunction = m_DepthInfo.DepthFunction;
    programDescriptor.PrimitiveType = primitiveType;

    auto program = GraphicsBackend::Current()->CreateProgram(programDescriptor);

    size_t hash = ShaderLocal::GetPSOHash(VertexAttributes::GetHash(vertexAttributes), colorFormat, isLinear, depthFormat, primitiveType);
    m_Programs[hash] = program;

    return m_Programs[hash];
}
