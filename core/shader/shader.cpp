#include "shader.h"
#include "shader_loader/shader_loader.h"
#include "graphics_backend_api.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_program_descriptor.h"
#include "enums/framebuffer_attachment.h"
#include "hash.h"
#include "editor/profiler/profiler.h"
#include "drawable_geometry/drawable_geometry.h"

#include <vector>

namespace ShaderLocal
{
    size_t GetStencilOperationDescriptorHash(const GraphicsBackendStencilOperationDescriptor& stencilOperationDescriptor)
    {
        size_t hash = 0;
        hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.FailOp));
        hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.DepthFailOp));
        hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.PassOp));
        hash = Hash::Combine(hash, std::hash<ComparisonFunction>{}(stencilOperationDescriptor.ComparisonFunction));
        return hash;
    }

    size_t GetStencilDescriptorHash(const GraphicsBackendStencilDescriptor& stencilDescriptor)
    {
        size_t hash = 0;
        hash = Hash::Combine(hash, std::hash<bool>{}(stencilDescriptor.Enabled));
        hash = Hash::Combine(hash, std::hash<uint8_t>{}(stencilDescriptor.ReadMask));
        hash = Hash::Combine(hash, std::hash<uint8_t>{}(stencilDescriptor.WriteMask));
        hash = Hash::Combine(hash, GetStencilOperationDescriptorHash(stencilDescriptor.FrontFaceOpDescriptor));
        hash = Hash::Combine(hash, GetStencilOperationDescriptorHash(stencilDescriptor.BackFaceOpDescriptor));
        return hash;
    }

    size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat, PrimitiveType primitiveType,
        const GraphicsBackendStencilDescriptor& stencilDescriptor)
    {
        size_t hash = Hash::Combine(std::hash<TextureInternalFormat>{}(colorTargetFormat), std::hash<TextureInternalFormat>{}(depthTargetFormat));
        hash = Hash::Combine(hash, std::hash<bool>{}(isLinear));
        hash = Hash::Combine(hash, std::hash<PrimitiveType>{}(primitiveType));
        hash = Hash::Combine(hash, GetStencilDescriptorHash(stencilDescriptor));
        hash = Hash::Combine(hash, vertexAttributesHash);
        return hash;
    }
}

std::shared_ptr<Shader> Shader::Load(const std::filesystem::path &path, const std::vector<std::string> &keywords,
    BlendInfo blendInfo, CullInfo cullInfo, GraphicsBackendDepthDescriptor depthDescriptor)
{
    Profiler::Marker _("Shader::Load", path.string());

    auto shader = ShaderLoader::Load(path, keywords, blendInfo, cullInfo, depthDescriptor);

    if (!shader)
    {
        static std::shared_ptr<Shader> fallback = ShaderLoader::Load("core_resources/shaders/fallback", {}, {}, {}, {});

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

Shader::Shader(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, GraphicsBackendDepthDescriptor depthDescriptor,
               std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
               std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
               std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
               std::string name, bool supportInstancing) :
    m_Shaders(std::move(shaders)),
    m_CullInfo(cullInfo),
    m_BlendInfo(blendInfo),
    m_DepthDescriptor(depthDescriptor),
    m_Name(std::move(name)),
    m_SupportInstancing(supportInstancing),
    m_Textures(std::move(textures)),
    m_Samplers(std::move(samplers)),
    m_Buffers(std::move(buffers))
{
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

const GraphicsBackendProgram& Shader::GetProgram(const std::shared_ptr<DrawableGeometry>& geometry)
{
    return GetProgram(geometry->GetVertexAttributes(), geometry->GetPrimitiveType());
}

const GraphicsBackendProgram& Shader::GetProgram(const VertexAttributes &vertexAttributes, PrimitiveType primitiveType)
{
    if (!GraphicsBackend::Current()->RequireStrictPSODescriptor() && !m_Programs.empty())
    {
        return m_Programs.begin()->second;
    }

    bool isLinear;
    const TextureInternalFormat colorTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::COLOR_ATTACHMENT0, &isLinear);
    const TextureInternalFormat depthTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, nullptr);

    const GraphicsBackendStencilDescriptor& stencilDescriptor = GraphicsBackend::Current()->GetStencilDescriptor();

    const size_t hash = ShaderLocal::GetPSOHash(vertexAttributes.GetHash(), colorTargetFormat, isLinear, depthTargetFormat, primitiveType, stencilDescriptor);

    const auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
    {
        return it->second;
    }

    return CreatePSO(m_Shaders, m_BlendInfo, colorTargetFormat, isLinear, depthTargetFormat, vertexAttributes.GetAttributes(), primitiveType, stencilDescriptor, m_Name);
}

const GraphicsBackendProgram& Shader::CreatePSO(std::vector<GraphicsBackendShaderObject>& shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat, bool isLinear,
	TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes, PrimitiveType primitiveType,
	const GraphicsBackendStencilDescriptor& stencilDescriptor,
	const std::string& name)
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
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.Name = &name;
    programDescriptor.ColorAttachmentDescriptor = colorAttachmentDescriptor;
    programDescriptor.DepthFormat = depthFormat;
    programDescriptor.CullFace = m_CullInfo.Face;
    programDescriptor.CullFaceOrientation = m_CullInfo.Orientation;
    programDescriptor.DepthDescriptor = m_DepthDescriptor;
    programDescriptor.StencilDescriptor = stencilDescriptor;
    programDescriptor.PrimitiveType = primitiveType;

    const GraphicsBackendProgram program = GraphicsBackend::Current()->CreateProgram(programDescriptor);

    const size_t hash = ShaderLocal::GetPSOHash(VertexAttributes::GetHash(vertexAttributes), colorFormat, isLinear, depthFormat, primitiveType, stencilDescriptor);
    m_Programs[hash] = program;

    return m_Programs[hash];
}
