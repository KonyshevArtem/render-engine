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
    size_t GetPSOHash(size_t vertexAttributesHash, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat, PrimitiveType primitiveType,
        const GraphicsBackendStencilDescriptor& stencilDescriptor,
        const GraphicsBackendDepthDescriptor& depthDescriptor,
        const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor,
        const GraphicsBackendBlendDescriptor& blendDescriptor)
    {
        size_t hash = 0;
        if (GraphicsBackend::Current()->RequireRTFormatsForPSO())
        {
            hash = Hash::Combine(hash, std::hash<TextureInternalFormat>{}(colorTargetFormat));
            hash = Hash::Combine(hash, std::hash<TextureInternalFormat>{}(depthTargetFormat));
            hash = Hash::Combine(hash, std::hash<bool>{}(isLinear));
        }
        if (GraphicsBackend::Current()->RequirePrimitiveTypeForPSO())
			hash = Hash::Combine(hash, std::hash<PrimitiveType>{}(primitiveType));
        if (GraphicsBackend::Current()->RequireStencilStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetStencilDescriptorHash(stencilDescriptor));
        if (GraphicsBackend::Current()->RequireDepthStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetDepthDescriptorHash(depthDescriptor));
        if (GraphicsBackend::Current()->RequireRasterizerStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetRasterizerDescriptorHash(rasterizerDescriptor));
        if (GraphicsBackend::Current()->RequireBlendStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetBlendDescriptorHash(blendDescriptor));
        if (GraphicsBackend::Current()->RequireVertexAttributesForPSO())
			hash = Hash::Combine(hash, vertexAttributesHash);
        return hash;
    }
}

std::shared_ptr<Shader> Shader::Load(const std::filesystem::path& path, const std::vector<std::string>& keywords)
{
    Profiler::Marker _("Shader::Load", path.string());

    auto shader = ShaderLoader::Load(path, keywords);

    if (!shader)
    {
        static std::shared_ptr<Shader> fallback = ShaderLoader::Load("core_resources/shaders/fallback", {});

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

Shader::Shader(std::vector<GraphicsBackendShaderObject> &shaders,
               std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
               std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
               std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
               std::string name, bool supportInstancing) :
    m_Shaders(std::move(shaders)),
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
    bool isLinear;
    const TextureInternalFormat colorTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::COLOR_ATTACHMENT0, &isLinear);
    const TextureInternalFormat depthTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, nullptr);

    const GraphicsBackendStencilDescriptor& stencilDescriptor = GraphicsBackend::Current()->GetStencilDescriptor();
    const GraphicsBackendDepthDescriptor& depthDescriptor = GraphicsBackend::Current()->GetDepthState();
    const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor = GraphicsBackend::Current()->GetRasterizerState();
    const GraphicsBackendBlendDescriptor& blendDescriptor = GraphicsBackend::Current()->GetBlendState();

    const size_t hash = ShaderLocal::GetPSOHash(vertexAttributes.GetHash(), colorTargetFormat, isLinear, depthTargetFormat, primitiveType, stencilDescriptor, depthDescriptor, rasterizerDescriptor, blendDescriptor);

    const auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
	    return it->second;

    GraphicsBackendColorAttachmentDescriptor colorAttachmentDescriptor{};
    colorAttachmentDescriptor.Format = colorTargetFormat;
    colorAttachmentDescriptor.BlendDescriptor = blendDescriptor;
    colorAttachmentDescriptor.IsLinear = isLinear;

    GraphicsBackendProgramDescriptor programDescriptor{};
    programDescriptor.Shaders = &m_Shaders;
    programDescriptor.VertexAttributes = &vertexAttributes.GetAttributes();
    programDescriptor.Textures = &m_Textures;
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.Name = &m_Name;
    programDescriptor.ColorAttachmentDescriptor = colorAttachmentDescriptor;
    programDescriptor.DepthFormat = depthTargetFormat;
    programDescriptor.RasterizerDescriptor = rasterizerDescriptor;
    programDescriptor.DepthDescriptor = depthDescriptor;
    programDescriptor.StencilDescriptor = stencilDescriptor;
    programDescriptor.PrimitiveType = primitiveType;

    const GraphicsBackendProgram program = GraphicsBackend::Current()->CreateProgram(programDescriptor);
    m_Programs[hash] = program;
    return m_Programs[hash];
}
