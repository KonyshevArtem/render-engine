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
    size_t GetPSOHash(size_t vertexAttributesHash, const GraphicsBackendProgramDescriptor& programDescriptor)
    {
        size_t hash = 0;
        if (GraphicsBackend::Current()->RequireRTFormatsForPSO())
        {
            for (const GraphicsBackendColorAttachmentDescriptor& descriptor : programDescriptor.ColorAttachmentDescriptors)
            {
	            hash = Hash::Combine(hash, std::hash<TextureInternalFormat>{}(descriptor.Format));
                hash = Hash::Combine(hash, std::hash<bool>{}(descriptor.IsLinear));
            }
            hash = Hash::Combine(hash, std::hash<TextureInternalFormat>{}(programDescriptor.DepthFormat));
        }
        if (GraphicsBackend::Current()->RequirePrimitiveTypeForPSO())
			hash = Hash::Combine(hash, std::hash<PrimitiveType>{}(programDescriptor.PrimitiveType));
        if (GraphicsBackend::Current()->RequireStencilStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetStencilDescriptorHash(programDescriptor.StencilDescriptor));
        if (GraphicsBackend::Current()->RequireDepthStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetDepthDescriptorHash(programDescriptor.DepthDescriptor));
        if (GraphicsBackend::Current()->RequireRasterizerStateForPSO())
			hash = Hash::Combine(hash, GraphicsBackendBase::GetRasterizerDescriptorHash(programDescriptor.RasterizerDescriptor));
        if (GraphicsBackend::Current()->RequireBlendStateForPSO())
        {
			for (const GraphicsBackendColorAttachmentDescriptor& descriptor : programDescriptor.ColorAttachmentDescriptors)
				hash = Hash::Combine(hash, GraphicsBackendBase::GetBlendDescriptorHash(descriptor.BlendDescriptor));
        }
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

Shader::Shader(std::vector<GraphicsBackendShaderObject>& shaders,
               std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
               std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
               std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
               std::unordered_map<std::string, GraphicsBackendTLASInfo> TLASes,
               ThreadGroupSize threadGroupSize,
               std::string name, bool supportInstancing) :
    m_Shaders(std::move(shaders)),
    m_Name(std::move(name)),
    m_SupportInstancing(supportInstancing),
    m_ThreadGroupSize(threadGroupSize),
    m_Textures(std::move(textures)),
    m_Samplers(std::move(samplers)),
    m_Buffers(std::move(buffers)),
	m_TLASes(std::move(TLASes))
{
    if (m_Shaders.size() == 1 && m_Shaders[0].Type == ShaderType::COMPUTE_SHADER)
        m_Type = ProgramType::COMPUTE;
    else
        m_Type = ProgramType::RENDER;
}

Shader::~Shader()
{
    for (const auto& pair : m_Programs)
	    GraphicsBackend::Current()->DeleteProgram(pair.second);

    for (const auto& shader : m_Shaders)
	    GraphicsBackend::Current()->DeleteShader(shader);
}

const GraphicsBackendProgram& Shader::GetProgram()
{
    return GetOrCreateComputeProgram();
}

const GraphicsBackendProgram& Shader::GetProgram(const std::shared_ptr<DrawableGeometry>& geometry)
{
    if (m_Type == ProgramType::COMPUTE)
        return GetOrCreateComputeProgram();
    return GetProgram(geometry->GetVertexAttributes(), geometry->GetPrimitiveType());
}

const GraphicsBackendProgram& Shader::GetProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType)
{
    if (m_Type == ProgramType::COMPUTE)
        return GetOrCreateComputeProgram();
    return GetOrCreateRenderProgram(vertexAttributes, primitiveType);
}

const GraphicsBackendProgram& Shader::GetOrCreateRenderProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType)
{
	constexpr int colorAttachmentsCount = static_cast<int>(FramebufferAttachment::COLOR_ATTACHMENTS_COUNT);

    GraphicsBackendProgramDescriptor programDescriptor{};
    programDescriptor.StencilDescriptor = GraphicsBackend::Current()->GetStencilDescriptor();
    programDescriptor.DepthDescriptor = GraphicsBackend::Current()->GetDepthState();
    programDescriptor.RasterizerDescriptor = GraphicsBackend::Current()->GetRasterizerState();
	programDescriptor.PrimitiveType = primitiveType;

    for (int i = 0; i < colorAttachmentsCount; ++i)
    {
        programDescriptor.ColorAttachmentDescriptors[i].Format = GraphicsBackend::Current()->GetRenderTargetFormat(static_cast<FramebufferAttachment>(i), &programDescriptor.ColorAttachmentDescriptors[i].IsLinear);
        programDescriptor.ColorAttachmentDescriptors[i].BlendDescriptor = GraphicsBackend::Current()->GetBlendState();
    }
    programDescriptor.DepthFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, nullptr);

    const size_t hash = ShaderLocal::GetPSOHash(vertexAttributes.GetHash(), programDescriptor);

    const auto it = m_Programs.find(hash);
    if (it != m_Programs.end())
        return it->second;

    programDescriptor.Type = m_Type;
    programDescriptor.Shaders = &m_Shaders;
    programDescriptor.VertexAttributes = &vertexAttributes.GetAttributes();
    programDescriptor.Textures = &m_Textures;
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.TLASes = &m_TLASes;
    programDescriptor.Name = &m_Name;

    const GraphicsBackendProgram program = GraphicsBackend::Current()->CreateProgram(programDescriptor);
    m_Programs[hash] = program;
    return m_Programs[hash];
}

const GraphicsBackendProgram& Shader::GetOrCreateComputeProgram()
{
    if (!m_Programs.empty())
        return m_Programs.begin()->second;

    GraphicsBackendProgramDescriptor programDescriptor{};
    programDescriptor.Type = m_Type;
    programDescriptor.Shaders = &m_Shaders;
    programDescriptor.Textures = &m_Textures;
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.TLASes = &m_TLASes;
    programDescriptor.Name = &m_Name;
    programDescriptor.ThreadGroupSize = m_ThreadGroupSize;

    const GraphicsBackendProgram program = GraphicsBackend::Current()->CreateProgram(programDescriptor);
    m_Programs[0] = program;
    return m_Programs[0];
}
