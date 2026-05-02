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

size_t Shader::s_GlobalDefinesHash;
std::mutex Shader::s_GlobalDefinesMutex;
std::unordered_set<std::string> Shader::s_GlobalDefines;

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

Shader::Shader(std::filesystem::path path, std::vector<std::string> defines) :
    m_Path(std::move(path)),
    m_Defines(std::move(defines))
{
    Init();
}

Shader::~Shader()
{
    DeInit();
}

void Shader::AddGlobalDefine(const std::string& define)
{
    std::lock_guard<std::mutex> lock(s_GlobalDefinesMutex);
    s_GlobalDefines.insert(define);
    s_GlobalDefinesHash = ShaderLoader::GetDefinesHash(s_GlobalDefines);
}

void Shader::RemoveGlobalDefine(const std::string& define)
{
    std::lock_guard<std::mutex> lock(s_GlobalDefinesMutex);
    s_GlobalDefines.erase(define);
    s_GlobalDefinesHash = ShaderLoader::GetDefinesHash(s_GlobalDefines);
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

void Shader::Init()
{
    DeInit();

    std::vector<std::string> defines;
    defines.reserve(m_Defines.size() + s_GlobalDefines.size());

    for (const std::string& define : m_Defines)
        defines.push_back(define);
    for (const std::string& define : s_GlobalDefines)
        defines.push_back(define);

    ShaderLoader::Load(m_Path, defines, m_Shaders, m_Textures, m_Buffers, m_Samplers, m_TLASes, m_ThreadGroupSize, m_Name, m_SupportInstancing);

    if (m_Shaders.size() == 1 && m_Shaders[0].Type == ShaderType::COMPUTE_SHADER)
        m_Type = ProgramType::COMPUTE;
    else
        m_Type = ProgramType::RENDER;

    m_LastGlobalDefinesHash = s_GlobalDefinesHash;
}

void Shader::DeInit()
{
    for (const auto& pair : m_Programs)
        GraphicsBackend::Current()->DeleteProgram(pair.second);

    for (const auto& shader : m_Shaders)
        GraphicsBackend::Current()->DeleteShader(shader);

    m_Programs.clear();
    m_Shaders.clear();
    m_Textures.clear();
    m_Buffers.clear();
    m_Samplers.clear();
    m_TLASes.clear();
}

void Shader::CheckGlobalDefines()
{
    if (s_GlobalDefinesHash != m_LastGlobalDefinesHash)
        Init();
}

const GraphicsBackendProgram& Shader::GetOrCreateRenderProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType)
{
    CheckGlobalDefines();

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

    const std::string& psoName = m_Name + "_" + std::to_string(hash);

    programDescriptor.Type = m_Type;
    programDescriptor.Shaders = &m_Shaders;
    programDescriptor.VertexAttributes = &vertexAttributes.GetAttributes();
    programDescriptor.Textures = &m_Textures;
    programDescriptor.Samplers = &m_Samplers;
    programDescriptor.Buffers = &m_Buffers;
    programDescriptor.TLASes = &m_TLASes;
    programDescriptor.Name = &psoName;

    const GraphicsBackendProgram program = GraphicsBackend::Current()->CreateProgram(programDescriptor);
    m_Programs[hash] = program;
    return m_Programs[hash];
}

const GraphicsBackendProgram& Shader::GetOrCreateComputeProgram()
{
    CheckGlobalDefines();

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
