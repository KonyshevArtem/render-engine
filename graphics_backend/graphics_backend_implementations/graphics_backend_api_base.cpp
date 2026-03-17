#include "graphics_backend_api_base.h"
#include "graphics_backend_api_opengl.h"
#include "graphics_backend_api_metal.h"
#include "graphics_backend_api_dx12.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_type.h"
#include "enums/framebuffer_attachment.h"
#include "enums/cubemap_face.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_program_descriptor.h"
#include "arguments.h"
#include "hash.h"

#include <functional>

namespace BaseBackendLocal
{
    constexpr int k_DeleteResourceDelay = 2;

    template<typename T>
    void DeleteResources(std::vector<std::pair<T, int>>& deletedResources, const std::function<void(T&)>& deleteFunction)
    {
        for (int i = deletedResources.size() - 1; i >= 0; --i)
        {
            auto& pair = deletedResources[i];
            if (--pair.second == 0)
            {
                deleteFunction(pair.first);
                std::swap(deletedResources[i], deletedResources.back());
                deletedResources.pop_back();
            }
        }
    }
}

GraphicsBackendBase *GraphicsBackendBase::Create()
{
    const bool openGL = Arguments::Contains("-opengl");
    const bool dx12 = Arguments::Contains("-dx12");
    const bool metal = Arguments::Contains("-metal");

#ifdef RENDER_BACKEND_OPENGL
#if RENDER_ENGINE_WINDOWS
    if (openGL && !dx12)
#endif
    {
        return new GraphicsBackendOpenGL();
    }
#endif

#ifdef RENDER_BACKEND_METAL
    return new GraphicsBackendMetal();
#endif

#ifdef RENDER_BACKEND_DX12
    return new GraphicsBackendDX12();
#endif

    return nullptr;
}

void GraphicsBackendBase::Init(void* data)
{
    m_MainThreadId = std::this_thread::get_id();
}

void GraphicsBackendBase::InitNewFrame()
{
    m_DrawCallCount = 0;

    BaseBackendLocal::DeleteResources<GraphicsBackendTexture>(m_DeletedTextures, [this](GraphicsBackendTexture& texture){ DeleteTexture_Internal(texture); });
    BaseBackendLocal::DeleteResources<GraphicsBackendSampler>(m_DeletedSamplers, [this](GraphicsBackendSampler& sampler){ DeleteSampler_Internal(sampler); });
    BaseBackendLocal::DeleteResources<GraphicsBackendBuffer>(m_DeletedBuffers, [this](GraphicsBackendBuffer& buffer){ DeleteBuffer_Internal(buffer); });
    BaseBackendLocal::DeleteResources<GraphicsBackendGeometry>(m_DeletedGeometries, [this](GraphicsBackendGeometry& geometry){ DeleteGeometry_Internal(geometry); });
    BaseBackendLocal::DeleteResources<GraphicsBackendShaderObject>(m_DeletedShaders, [this](GraphicsBackendShaderObject& shader){ DeleteShader_Internal(shader); });
    BaseBackendLocal::DeleteResources<GraphicsBackendProgram>(m_DeletedPrograms, [this](GraphicsBackendProgram& program){ DeleteProgram_Internal(program); });
}

void GraphicsBackendBase::IncrementFrameNumber()
{
    ++m_FrameCount;
}

uint64_t GraphicsBackendBase::GetFrameNumber() const
{
    return m_FrameCount;
}

void GraphicsBackendBase::DeleteTexture(const GraphicsBackendTexture& texture)
{
    m_DeletedTextures.emplace_back(texture, BaseBackendLocal::k_DeleteResourceDelay);

    auto Predicate = [&texture](const std::pair<uint32_t, GraphicsBackendTexture>& pair)
    {
        return pair.second.Texture == texture.Texture;
    };

    std::erase_if(m_BoundTextures, Predicate);
    std::erase_if(m_BoundRWTextures, Predicate);
}

void GraphicsBackendBase::DeleteSampler(const GraphicsBackendSampler& sampler)
{
    m_DeletedSamplers.emplace_back(sampler, BaseBackendLocal::k_DeleteResourceDelay);

    std::erase_if(m_BoundSamplers, [&sampler](const auto& pair)
    {
        return pair.second.Sampler == sampler.Sampler;
    });
}

void GraphicsBackendBase::DeleteBuffer(const GraphicsBackendBuffer& buffer)
{
    m_DeletedBuffers.emplace_back(buffer, BaseBackendLocal::k_DeleteResourceDelay);

    auto Predicate = [&buffer](const std::pair<uint32_t, BufferBindInfo>& pair)
    {
        return pair.second.Buffer.Buffer == buffer.Buffer;
    };

    std::erase_if(m_BoundBuffers, Predicate);
    std::erase_if(m_BoundConstantBuffers, Predicate);
    std::erase_if(m_BoundRWBuffers, Predicate);
}

void GraphicsBackendBase::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
    m_DeletedGeometries.emplace_back(geometry, BaseBackendLocal::k_DeleteResourceDelay);
}

void GraphicsBackendBase::DeleteShader(GraphicsBackendShaderObject shader)
{
    m_DeletedShaders.emplace_back(shader, BaseBackendLocal::k_DeleteResourceDelay);
}

void GraphicsBackendBase::DeleteProgram(GraphicsBackendProgram program)
{
    m_DeletedPrograms.emplace_back(program, BaseBackendLocal::k_DeleteResourceDelay);
}

void GraphicsBackendBase::BindResources(const GraphicsBackendProgram& program)
{
    auto HasBinding = [](uint32_t bindingFlags, uint32_t binding)
    {
        return (bindingFlags & (1 << binding)) != 0;
    };

    for (const auto& pair: m_BoundTextures)
    {
        if (HasBinding(program.TextureBindings, pair.first))
            BindTexture_Internal(pair.second, pair.first);
    }

    for (const auto& pair : m_BoundRWTextures)
    {
        if (HasBinding(program.RWTextureBindings, pair.first))
            BindRWTexture_Internal(pair.second, pair.first);
    }

    for (const auto& pair: m_BoundSamplers)
    {
        if (HasBinding(program.SamplerBindings, pair.first))
            BindSampler_Internal(pair.second, pair.first);
    }

    for (const auto& pair: m_BoundBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.BufferBindings, pair.first))
            BindBuffer_Internal(info.Buffer, info.Type, pair.first, info.Offset, info.Size, info.ElementsCount, info.Format);
    }

    for (const auto& pair: m_BoundConstantBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.ConstantBufferBindings, pair.first))
            BindConstantBuffer_Internal(info.Buffer, pair.first, info.Offset, info.Size);
    }

    for (const auto& pair : m_BoundRWBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.RWBufferBindings, pair.first))
            BindRWBuffer_Internal(info.Buffer, info.Type, pair.first, info.Offset, info.Size, info.ElementsCount, info.Format);
    }
}

void GraphicsBackendBase::BindTexture(const GraphicsBackendTexture& texture, uint32_t index)
{
    m_BoundTextures[index] = texture;
}

void GraphicsBackendBase::BindSampler(const GraphicsBackendSampler& sampler, uint32_t index)
{
    m_BoundSamplers[index] = sampler;
}

void GraphicsBackendBase::BindTextureSampler(const GraphicsBackendTexture& texture, const GraphicsBackendSampler& sampler, uint32_t index)
{
    BindTexture(texture, index);
    BindSampler(sampler, index);
}

void GraphicsBackendBase::BindRWTexture(const GraphicsBackendTexture& texture, uint32_t index)
{
    m_BoundRWTextures[index] = texture;
}

void GraphicsBackendBase::UploadImagePixels(const GraphicsBackendTexture& texture, int level, int width, int height, int depth, int imageSize, const void* pixelsData)
{
    UploadImagePixels(texture, level, CubemapFace::POSITIVE_X, width, height, depth, imageSize, pixelsData);
}

void GraphicsBackendBase::BindBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    m_BoundBuffers[index] = BufferBindInfo{ buffer, BufferType::BYTE_ADDRESS_BUFFER, offset, size };
}

void GraphicsBackendBase::BindBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int elementsCount)
{
    m_BoundBuffers[index] = BufferBindInfo{ buffer, BufferType::STRUCTURED_BUFFER, offset, size, elementsCount };
}

void GraphicsBackendBase::BindBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int elementsCount, TextureInternalFormat format)
{
    m_BoundBuffers[index] = BufferBindInfo{ buffer, BufferType::TYPED_BUFFER, offset, 0, elementsCount, format };
}

void GraphicsBackendBase::BindConstantBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    m_BoundConstantBuffers[index] = BufferBindInfo{buffer, BufferType::CONSTANT_BUFFER, offset, size};
}

void GraphicsBackendBase::BindRWBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    m_BoundRWBuffers[index] = BufferBindInfo{ buffer, BufferType::BYTE_ADDRESS_BUFFER, offset, size };
}

void GraphicsBackendBase::BindRWBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int elementsCount)
{
    m_BoundRWBuffers[index] = BufferBindInfo{ buffer, BufferType::STRUCTURED_BUFFER, offset, size, elementsCount };
}

void GraphicsBackendBase::BindRWBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int elementsCount, TextureInternalFormat format)
{
    m_BoundRWBuffers[index] = BufferBindInfo{ buffer, BufferType::TYPED_BUFFER, offset, 0, elementsCount, format };
}

GraphicsBackendProgram GraphicsBackendBase::CreateProgram(uint64_t programPtr, const GraphicsBackendProgramDescriptor& descriptor)
{
    GraphicsBackendProgram program{};
    program.Program = programPtr;
    program.Type = descriptor.Type;
    program.BufferBindings = 0;
    program.TextureBindings = 0;
    program.SamplerBindings = 0;
    program.RWTextureBindings = 0;
    program.RWBufferBindings = 0;

    for (const auto& pair : *descriptor.Buffers)
    {
        const std::shared_ptr<GraphicsBackendBufferInfo>& buffer = pair.second;
        if (buffer->GetBufferType() == BufferType::CONSTANT_BUFFER)
            program.ConstantBufferBindings |= 1 << buffer->GetBinding();
        else if (buffer->GetReadWrite())
            program.RWBufferBindings |= 1 << buffer->GetBinding();
        else
            program.BufferBindings |= 1 << buffer->GetBinding();
    }

    for (const auto& pair : *descriptor.Textures)
    {
        const GraphicsBackendTextureInfo& texture = pair.second;
        if (texture.ReadWrite)
            program.RWTextureBindings |= 1 << texture.Binding;
        else
			program.TextureBindings |= 1 << texture.Binding;
    }

    for (const auto& pair : *descriptor.Samplers)
        program.SamplerBindings |= 1 << pair.second.Binding;

    return program;
}

void GraphicsBackendBase::EndRenderPass()
{
    m_StencilDescriptor = {};
    m_DepthDescriptor = {};
    m_RasterizerDescriptor = {};
    m_BlendDescriptor = {};
}

void GraphicsBackendBase::SetStencilState(const GraphicsBackendStencilDescriptor& stencilDescriptor)
{
    m_StencilDescriptor = stencilDescriptor;
}

const GraphicsBackendStencilDescriptor& GraphicsBackendBase::GetStencilDescriptor() const
{
    return m_StencilDescriptor;
}

void GraphicsBackendBase::SetDepthState(const GraphicsBackendDepthDescriptor& depthDescriptor)
{
    m_DepthDescriptor = depthDescriptor;
}

const GraphicsBackendDepthDescriptor& GraphicsBackendBase::GetDepthState() const
{
    return m_DepthDescriptor;
}

void GraphicsBackendBase::SetRasterizerState(const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor)
{
    m_RasterizerDescriptor = rasterizerDescriptor;
}

const GraphicsBackendRasterizerDescriptor& GraphicsBackendBase::GetRasterizerState() const
{
    return m_RasterizerDescriptor;
}

void GraphicsBackendBase::SetBlendState(const GraphicsBackendBlendDescriptor& blendDescriptor)
{
    m_BlendDescriptor = blendDescriptor;
}

const GraphicsBackendBlendDescriptor& GraphicsBackendBase::GetBlendState() const
{
    return m_BlendDescriptor;
}

bool GraphicsBackendBase::IsTexture3D(TextureType type)
{
    return type == TextureType::TEXTURE_2D_ARRAY || type == TextureType::TEXTURE_3D || type == TextureType::TEXTURE_CUBEMAP_ARRAY;
}

bool GraphicsBackendBase::IsCompressedTextureFormat(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::R8:
        case TextureInternalFormat::R8_SNORM:
        case TextureInternalFormat::R16:
        case TextureInternalFormat::R16_SNORM:
        case TextureInternalFormat::RG8:
        case TextureInternalFormat::RG8_SNORM:
        case TextureInternalFormat::RG16:
        case TextureInternalFormat::RG16_SNORM:
        case TextureInternalFormat::R3_G3_B2:
        case TextureInternalFormat::RGB4:
        case TextureInternalFormat::RGB5:
        case TextureInternalFormat::RGB8:
        case TextureInternalFormat::RGB8_SNORM:
        case TextureInternalFormat::RGB10:
        case TextureInternalFormat::RGB12:
        case TextureInternalFormat::RGB16:
        case TextureInternalFormat::RGBA2:
        case TextureInternalFormat::RGBA4:
        case TextureInternalFormat::RGB5_A1:
        case TextureInternalFormat::RGBA8:
        case TextureInternalFormat::RGBA8_SNORM:
        case TextureInternalFormat::RGB10_A2:
        case TextureInternalFormat::RGB10_A2UI:
        case TextureInternalFormat::RGBA12:
        case TextureInternalFormat::RGBA16:
        case TextureInternalFormat::R16F:
        case TextureInternalFormat::RG16F:
        case TextureInternalFormat::RGB16F:
        case TextureInternalFormat::RGBA16F:
        case TextureInternalFormat::R32F:
        case TextureInternalFormat::RG32F:
        case TextureInternalFormat::RGB32F:
        case TextureInternalFormat::RGBA32F:
        case TextureInternalFormat::R11F_G11F_B10F:
        case TextureInternalFormat::RGB9_E5:
        case TextureInternalFormat::R8I:
        case TextureInternalFormat::R8UI:
        case TextureInternalFormat::R16I:
        case TextureInternalFormat::R16UI:
        case TextureInternalFormat::R32I:
        case TextureInternalFormat::R32UI:
        case TextureInternalFormat::RG8I:
        case TextureInternalFormat::RG8UI:
        case TextureInternalFormat::RG16I:
        case TextureInternalFormat::RG16UI:
        case TextureInternalFormat::RG32I:
        case TextureInternalFormat::RG32UI:
        case TextureInternalFormat::RGB8I:
        case TextureInternalFormat::RGB8UI:
        case TextureInternalFormat::RGB16I:
        case TextureInternalFormat::RGB16UI:
        case TextureInternalFormat::RGB32I:
        case TextureInternalFormat::RGB32UI:
        case TextureInternalFormat::RGBA8I:
        case TextureInternalFormat::RGBA8UI:
        case TextureInternalFormat::RGBA16I:
        case TextureInternalFormat::RGBA16UI:
        case TextureInternalFormat::RGBA32I:
        case TextureInternalFormat::RGBA32UI:
        case TextureInternalFormat::DEPTH_32:
        case TextureInternalFormat::DEPTH_24:
        case TextureInternalFormat::DEPTH_16:
        case TextureInternalFormat::DEPTH_32_STENCIL_8:
        case TextureInternalFormat::DEPTH_24_STENCIL_8:
            return false;
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC4:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
        case TextureInternalFormat::ASTC_5X5:
        case TextureInternalFormat::ASTC_6X6:
        case TextureInternalFormat::ASTC_8X8:
        case TextureInternalFormat::ASTC_10X10:
        case TextureInternalFormat::ASTC_12X12:
            return true;
    }
}

int GraphicsBackendBase::GetBlockSize(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC4:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
            return 4;
        case TextureInternalFormat::ASTC_5X5:
            return 5;
        case TextureInternalFormat::ASTC_6X6:
            return 6;
        case TextureInternalFormat::ASTC_8X8:
            return 8;
        case TextureInternalFormat::ASTC_10X10:
            return 10;
        case TextureInternalFormat::ASTC_12X12:
            return 12;
        default:
            return 0;
    }
}

int GraphicsBackendBase::GetBlockBytes(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC4:
            return 8;
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
        case TextureInternalFormat::ASTC_5X5:
        case TextureInternalFormat::ASTC_6X6:
        case TextureInternalFormat::ASTC_8X8:
        case TextureInternalFormat::ASTC_10X10:
        case TextureInternalFormat::ASTC_12X12:
            return 16;
        default:
            return 0;
    }
}

bool GraphicsBackendBase::IsDepthFormat(TextureInternalFormat format)
{
    return format == TextureInternalFormat::DEPTH_16 ||
            format == TextureInternalFormat::DEPTH_24 ||
            format == TextureInternalFormat::DEPTH_32 ||
            format == TextureInternalFormat::DEPTH_24_STENCIL_8 ||
            format == TextureInternalFormat::DEPTH_32_STENCIL_8;
}

bool GraphicsBackendBase::IsDepthAttachment(FramebufferAttachment attachment)
{
    return attachment == FramebufferAttachment::DEPTH_ATTACHMENT ||
            attachment == FramebufferAttachment::STENCIL_ATTACHMENT ||
            attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
}

bool GraphicsBackendBase::IsMainThread()
{
    return std::this_thread::get_id() == m_MainThreadId;
}

size_t GraphicsBackendBase::GetDepthDescriptorHash(const GraphicsBackendDepthDescriptor& depthDescriptor)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<bool>{}(depthDescriptor.Enabled));
    hash = Hash::Combine(hash, std::hash<bool>{}(depthDescriptor.WriteDepth));
    hash = Hash::Combine(hash, std::hash<ComparisonFunction>{}(depthDescriptor.DepthFunction));
    return hash;
}

size_t GraphicsBackendBase::GetStencilOperationDescriptorHash(const GraphicsBackendStencilOperationDescriptor& stencilOperationDescriptor)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.FailOp));
    hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.DepthFailOp));
    hash = Hash::Combine(hash, std::hash<StencilOperation>{}(stencilOperationDescriptor.PassOp));
    hash = Hash::Combine(hash, std::hash<ComparisonFunction>{}(stencilOperationDescriptor.ComparisonFunction));
    return hash;
}

size_t GraphicsBackendBase::GetStencilDescriptorHash(const GraphicsBackendStencilDescriptor& stencilDescriptor)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<bool>{}(stencilDescriptor.Enabled));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(stencilDescriptor.ReadMask));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(stencilDescriptor.WriteMask));
    hash = Hash::Combine(hash, GetStencilOperationDescriptorHash(stencilDescriptor.FrontFaceOpDescriptor));
    hash = Hash::Combine(hash, GetStencilOperationDescriptorHash(stencilDescriptor.BackFaceOpDescriptor));
    return hash;
}

size_t GraphicsBackendBase::GetRasterizerDescriptorHash(const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<CullFace>{}(rasterizerDescriptor.Face));
    hash = Hash::Combine(hash, std::hash<CullFaceOrientation>{}(rasterizerDescriptor.Orientation));
    return hash;
}

size_t GraphicsBackendBase::GetBlendDescriptorHash(const GraphicsBackendBlendDescriptor& blendDescriptor)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<bool>{}(blendDescriptor.Enabled));
    hash = Hash::Combine(hash, std::hash<BlendFactor>{}(blendDescriptor.SourceFactor));
    hash = Hash::Combine(hash, std::hash<BlendFactor>{}(blendDescriptor.DestinationFactor));
    hash = Hash::Combine(hash, std::hash<ColorWriteMask>{}(blendDescriptor.ColorWriteMask));
    return hash;
}

std::string GraphicsBackendBase::GetShaderTypeName(ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderType::VERTEX_SHADER:
        return "vs";
    case ShaderType::FRAGMENT_SHADER:
        return "ps";
    case ShaderType::COMPUTE_SHADER:
        return "cs";
    default:
        return "Unknown";
    }
}
