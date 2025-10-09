#include "graphics_backend_api_base.h"
#include "graphics_backend_api_opengl.h"
#include "graphics_backend_api_metal.h"
#include "graphics_backend_api_dx12.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_type.h"
#include "enums/framebuffer_attachment.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_program_descriptor.h"
#include "arguments.h"

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
    if (metal)
    {
        return new GraphicsBackendMetal();
    }
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

    std::erase_if(m_BoundTextures, [&texture](const auto& pair)
    {
        return pair.second.Texture == texture.Texture;
    });
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
    std::erase_if(m_BoundStructuredBuffers, Predicate);
    std::erase_if(m_BoundConstantBuffers, Predicate);
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

    for (const auto& pair: m_BoundSamplers)
    {
        if (HasBinding(program.SamplerBindings, pair.first))
            BindSampler_Internal(pair.second, pair.first);
    }

    for (const auto& pair: m_BoundBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.BufferBindings, pair.first))
            BindBuffer_Internal(info.Buffer, pair.first, info.Offset, info.Size);
    }

    for (const auto& pair: m_BoundStructuredBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.BufferBindings, pair.first))
            BindStructuredBuffer_Internal(info.Buffer, pair.first, info.Offset, info.Size, info.ElementsCount);
    }

    for (const auto& pair: m_BoundConstantBuffers)
    {
        const BufferBindInfo& info = pair.second;
        if (HasBinding(program.ConstantBufferBindings, pair.first))
            BindConstantBuffer_Internal(info.Buffer, pair.first, info.Offset, info.Size);
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

void GraphicsBackendBase::BindBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    m_BoundBuffers[index] = BufferBindInfo{buffer, offset, size};
}

void GraphicsBackendBase::BindStructuredBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int count)
{
    m_BoundStructuredBuffers[index] = BufferBindInfo{buffer, offset, size, count};
}

void GraphicsBackendBase::BindConstantBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    m_BoundConstantBuffers[index] = BufferBindInfo{buffer, offset, size};
}

GraphicsBackendProgram GraphicsBackendBase::CreateProgram(uint64_t programPtr, const GraphicsBackendProgramDescriptor& descriptor)
{
    GraphicsBackendProgram program{};
    program.Program = programPtr;
    program.BufferBindings = 0;
    program.TextureBindings = 0;
    program.SamplerBindings = 0;

    for (const auto& pair : *descriptor.Buffers)
    {
        if (pair.second->GetBufferType() == BufferType::CONSTANT_BUFFER)
            program.ConstantBufferBindings |= 1 << pair.second->GetBinding();
        else
            program.BufferBindings |= 1 << pair.second->GetBinding();
    }

    for (const auto& pair : *descriptor.Textures)
        program.TextureBindings |= 1 << pair.second.Binding;

    for (const auto& pair : *descriptor.Samplers)
        program.SamplerBindings |= 1 << pair.second.Binding;

    return program;
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
