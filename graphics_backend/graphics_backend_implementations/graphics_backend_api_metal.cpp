#ifdef RENDER_BACKEND_METAL

#include "graphics_backend_api_metal.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/framebuffer_attachment.h"
#include "enums/fence_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_depth_stencil_state.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "helpers/metal_helpers.h"

#include "Metal/Metal.hpp"

NS::Error *s_Error;
const int s_MaxBuffers = 31;
MTL::ResourceOptions s_DefaultBufferStorageMode;

struct MetalInitData
{
    MTL::Device* Device;
    MTL::CommandBuffer* RenderCommandBuffer;
    MTL::CommandBuffer* CopyCommandBuffer;
};

struct MetalFrameData
{
    MTL::CommandBuffer* RenderCommandBuffer;
    MTL::CommandBuffer* CopyCommandBuffer;
    MTL::RenderPassDescriptor* BackbufferDescriptor;
};

struct BufferData
{
    MTL::Buffer* Buffer;
    MTL::ResourceOptions StorageMode;
};

void GraphicsBackendMetal::Init(void *data)
{
    auto metalData = reinterpret_cast<MetalInitData*>(data);
    m_Device = metalData->Device;
    m_RenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    SetCommandBuffers(metalData->RenderCommandBuffer, metalData->CopyCommandBuffer);

    s_DefaultBufferStorageMode = m_Device->hasUnifiedMemory() ? MTL::ResourceStorageModeShared : MTL::ResourceStorageModePrivate;
}

GraphicsBackendName GraphicsBackendMetal::GetName()
{
    return GraphicsBackendName::METAL;
}

void GraphicsBackendMetal::InitNewFrame(void *data)
{
    auto metalData = reinterpret_cast<MetalFrameData*>(data);
    m_BackbufferDescriptor = metalData->BackbufferDescriptor;
    SetCommandBuffers(metalData->RenderCommandBuffer, metalData->CopyCommandBuffer);
}

GraphicsBackendTexture GraphicsBackendMetal::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name)
{
    MTL::TextureUsage textureUsage = MTL::TextureUsageShaderRead;
    if (isRenderTarget)
    {
        textureUsage |= MTL::TextureUsageRenderTarget;
    }

    MTL::TextureDescriptor *descriptor = MTL::TextureDescriptor::alloc()->init();
    descriptor->setWidth(width);
    descriptor->setHeight(height);
    descriptor->setPixelFormat(MetalHelpers::ToTextureInternalFormat(format, isLinear));
    descriptor->setTextureType(MetalHelpers::ToTextureType(type));
    descriptor->setStorageMode(MTL::StorageModePrivate);
    descriptor->setUsage(textureUsage);
    descriptor->setMipmapLevelCount(mipLevels);

    const bool isTextureArray = type == TextureType::TEXTURE_1D_ARRAY ||
                                type == TextureType::TEXTURE_2D_ARRAY ||
                                type == TextureType::TEXTURE_2D_MULTISAMPLE_ARRAY ||
                                type == TextureType::TEXTURE_CUBEMAP_ARRAY;

    if (type == TextureType::TEXTURE_3D)
    {
        descriptor->setDepth(depth);
    }
    else if (isTextureArray)
    {
        descriptor->setArrayLength(depth);
    }

    MTL::Texture* metalTexture = m_Device->newTexture(descriptor);
    descriptor->release();

    if (!name.empty())
    {
        metalTexture->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    GraphicsBackendTexture texture{};
    texture.Texture = reinterpret_cast<uint64_t>(metalTexture);
    texture.Type = type;
    texture.Format = format;
    texture.IsLinear = isLinear;
    return texture;
}

GraphicsBackendSampler GraphicsBackendMetal::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, const std::string& name)
{
    auto descriptor = MTL::SamplerDescriptor::alloc()->init();

    auto wrap = MetalHelpers::ToTextureWrapMode(wrapMode);
    descriptor->setRAddressMode(wrap);
    descriptor->setSAddressMode(wrap);
    descriptor->setTAddressMode(wrap);

    auto filtering = MetalHelpers::ToTextureFilteringMode(filteringMode);
    descriptor->setMinFilter(filtering);
    descriptor->setMagFilter(filtering);

    if (borderColor != nullptr)
    {
        auto border = MetalHelpers::ToTextureBorderColor(borderColor);
        descriptor->setBorderColor(border);
    }

    descriptor->setLodMinClamp(minLod);
    descriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterNearest);

    if (!name.empty())
    {
        descriptor->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    auto metalSampler = m_Device->newSamplerState(descriptor);
    descriptor->release();

    GraphicsBackendSampler sampler{};
    sampler.Sampler = reinterpret_cast<uint64_t>(metalSampler);
    return sampler;
}

void GraphicsBackendMetal::DeleteTexture(const GraphicsBackendTexture &texture)
{
    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    metalTexture->release();
}

void GraphicsBackendMetal::DeleteSampler(const GraphicsBackendSampler &sampler)
{
    auto metalSampler = reinterpret_cast<MTL::SamplerState*>(sampler.Sampler);
    metalSampler->release();
}

void GraphicsBackendMetal::BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);

    if (bindings.VertexIndex >= 0)
    {
        m_RenderCommandEncoder->setVertexTexture(metalTexture, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_RenderCommandEncoder->setFragmentTexture(metalTexture, bindings.FragmentIndex);
    }
}

void GraphicsBackendMetal::BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto metalSampler = reinterpret_cast<MTL::SamplerState *>(sampler.Sampler);

    if (bindings.VertexIndex >= 0)
    {
        m_RenderCommandEncoder->setVertexSamplerState(metalSampler, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_RenderCommandEncoder->setFragmentSamplerState(metalSampler, bindings.FragmentIndex);
    }
}

void GraphicsBackendMetal::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    assert(m_BlitCommandEncoder != nullptr);

    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    if (!metalTexture)
        return;

    m_BlitCommandEncoder->generateMipmaps(metalTexture);
}

void GraphicsBackendMetal::UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    int bytesPerRow;
    if (IsCompressedTextureFormat(texture.Format))
    {
        int blockSize = GetBlockSize(texture.Format);
        int blocksPerRow = (width + (blockSize - 1)) / blockSize;
        bytesPerRow = blocksPerRow * GetBlockBytes(texture.Format);
    }
    else
    {
        bytesPerRow = imageSize / height;
    }

    MTL::Texture* metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    MTL::Buffer* tempBuffer = m_Device->newBuffer(pixelsData, imageSize, s_DefaultBufferStorageMode);
    BeginCopyPass("Upload Texture Data");
    m_BlitCommandEncoder->copyFromBuffer(tempBuffer, 0, bytesPerRow, imageSize, MTL::Size::Make(width, height, 1), metalTexture, static_cast<NS::UInteger>(cubemapFace), level, MTL::Origin::Make(0, 0, depth));
    EndCopyPass();

    tempBuffer->release();
}

void GraphicsBackendMetal::AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor)
{
    auto metalTexture = GetTextureFromDescriptor(descriptor);

    auto configureDescriptor = [metalTexture, &descriptor](MTL::RenderPassAttachmentDescriptor* desc)
    {
        desc->setTexture(metalTexture);
        desc->setLevel(descriptor.Level);
        desc->setSlice(descriptor.Layer);
        desc->setLoadAction(MetalHelpers::ToLoadAction(descriptor.LoadAction));
        desc->setStoreAction(MetalHelpers::ToStoreAction(descriptor.StoreAction));
    };

    const bool isDepthStencil = descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
    const bool isDepth = descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT || isDepthStencil;
    const bool isStencil = descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT || isDepthStencil;

    if (isDepth || isStencil)
    {
        if (isDepth)
            configureDescriptor(m_RenderPassDescriptor->depthAttachment());

        if (isStencil)
            configureDescriptor(m_RenderPassDescriptor->stencilAttachment());
        else
            m_RenderPassDescriptor->stencilAttachment()->setTexture(nullptr);
    }
    else
    {
        const int index = static_cast<int>(descriptor.Attachment);
        configureDescriptor(m_RenderPassDescriptor->colorAttachments()->object(index));
    }
}

TextureInternalFormat GraphicsBackendMetal::GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear)
{
    bool isDepth = attachment == FramebufferAttachment::DEPTH_ATTACHMENT || attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
    bool isStencil = attachment == FramebufferAttachment::STENCIL_ATTACHMENT || attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;

    bool isLinear;
    if (isDepth)
    {
        return MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->depthAttachment()->texture()->pixelFormat(), isLinear);
    }
    if (isStencil)
    {
        return MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->stencilAttachment()->texture()->pixelFormat(), isLinear);
    }

    int index = static_cast<int>(attachment);
    TextureInternalFormat format = MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->colorAttachments()->object(index)->texture()->pixelFormat(), isLinear);
    if (outIsLinear)
        *outIsLinear = isLinear;
    return format;
}

GraphicsBackendBuffer GraphicsBackendMetal::CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data)
{
    const MTL::ResourceOptions storageMode = allowCPUWrites ? s_DefaultBufferStorageMode : MTL::ResourceStorageModePrivate;
    MTL::Buffer* metalBuffer = allowCPUWrites && data ? m_Device->newBuffer(data, size, storageMode) : m_Device->newBuffer(size, storageMode);

    if (!allowCPUWrites)
    {
        MTL::Buffer* tempBuffer = m_Device->newBuffer(data, size, s_DefaultBufferStorageMode);
        BeginCopyPass("Init Buffer " + name);
        m_BlitCommandEncoder->copyFromBuffer(tempBuffer, 0, metalBuffer, 0, size);
        EndCopyPass();

        tempBuffer->release();
    }

    if (!name.empty())
    {
        metalBuffer->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    BufferData* bufferData = new BufferData();
    bufferData->Buffer = metalBuffer;
    bufferData->StorageMode = storageMode;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(bufferData);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendMetal::DeleteBuffer(const GraphicsBackendBuffer &buffer)
{
    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);
    bufferData->Buffer->release();
    delete bufferData;
}

void GraphicsBackendMetal::BindBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    assert(m_RenderCommandEncoder != nullptr);

    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);
    if (bindings.VertexIndex >= 0)
    {
        m_RenderCommandEncoder->setVertexBuffer(bufferData->Buffer, offset, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_RenderCommandEncoder->setFragmentBuffer(bufferData->Buffer, offset, bindings.FragmentIndex);
    }
}

void GraphicsBackendMetal::BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    BindBuffer(buffer, bindings, offset, size);
}

void GraphicsBackendMetal::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data)
{
    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);
    uint8_t* contents = static_cast<uint8_t*>(bufferData->Buffer->contents()) + offset;
    memcpy(contents, data, size);

    if (bufferData->StorageMode == MTL::ResourceStorageModeManaged)
        bufferData->Buffer->didModifyRange(NS::Range::Make(offset, size));
}

void GraphicsBackendMetal::CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size)
{
    assert(m_BlitCommandEncoder != nullptr);

    const BufferData* sourceBufferData = reinterpret_cast<BufferData*>(source.Buffer);
    const BufferData* destinationBufferData = reinterpret_cast<BufferData*>(destination.Buffer);
    m_BlitCommandEncoder->copyFromBuffer(sourceBufferData->Buffer, sourceOffset, destinationBufferData->Buffer, destinationOffset, size);
}

uint64_t GraphicsBackendMetal::GetMaxConstantBufferSize()
{
    static uint64_t size = 0;

    if (size == 0)
    {
        size = m_Device->maxBufferLength();
    }

    return size;
}

int GraphicsBackendMetal::GetConstantBufferOffsetAlignment()
{
    // https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
    return 4;
}

GraphicsBackendGeometry GraphicsBackendMetal::CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name)
{
    GraphicsBackendGeometry geometry{};
    geometry.VertexBuffer = vertexBuffer;
    geometry.IndexBuffer = indexBuffer;
    return geometry;
}

void GraphicsBackendMetal::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
    DeleteBuffer(geometry.VertexBuffer);
    DeleteBuffer(geometry.IndexBuffer);
}

void GraphicsBackendMetal::SetCullFace(CullFace cullFace)
{
    assert(m_RenderCommandEncoder != nullptr);
    m_RenderCommandEncoder->setCullMode(MetalHelpers::ToCullFace(cullFace));
}

void GraphicsBackendMetal::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    assert(m_RenderCommandEncoder != nullptr);
    m_RenderCommandEncoder->setFrontFacingWinding(MetalHelpers::ToCullFaceOrientation(orientation));
}

void GraphicsBackendMetal::SetViewport(int x, int y, int width, int height, float near, float far)
{
    assert(m_RenderCommandEncoder != nullptr);

    MTL::Viewport viewport {static_cast<double>(x), static_cast<double>(y), static_cast<double>(width), static_cast<double>(height), near, far} ;
    m_RenderCommandEncoder->setViewport(viewport);
}

GraphicsBackendShaderObject GraphicsBackendMetal::CompileShader(ShaderType shaderType, const std::string &source, const std::string& name)
{
    auto library = m_Device->newLibrary(NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr, &s_Error);
    if (!library)
    {
        throw std::runtime_error(s_Error->localizedDescription()->utf8String());
    }

    if (!name.empty())
    {
        library->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = reinterpret_cast<uint64_t>(library);
    return shaderObject;
}

GraphicsBackendProgram GraphicsBackendMetal::CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat,
                                                           const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name)
{
    assert(shaders.size() == 2);

    MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();

    MTL::PixelFormat metalColorFormat = MetalHelpers::ToTextureInternalFormat(colorAttachmentDescriptor.Format, colorAttachmentDescriptor.IsLinear);
    MTL::PixelFormat metalDepthFormat = MetalHelpers::ToTextureInternalFormat(depthFormat, false);

    auto attachmentDesc = desc->colorAttachments()->object(0);
    attachmentDesc->setPixelFormat(metalColorFormat);
    attachmentDesc->setBlendingEnabled(colorAttachmentDescriptor.BlendingEnabled);
    attachmentDesc->setSourceRGBBlendFactor(MetalHelpers::ToBlendFactor(colorAttachmentDescriptor.SourceFactor));
    attachmentDesc->setDestinationRGBBlendFactor(MetalHelpers::ToBlendFactor(colorAttachmentDescriptor.DestinationFactor));

    desc->setDepthAttachmentPixelFormat(metalDepthFormat);

    if (depthFormat == TextureInternalFormat::DEPTH_32_STENCIL_8 || depthFormat == TextureInternalFormat::DEPTH_24_STENCIL_8)
        desc->setStencilAttachmentPixelFormat(metalDepthFormat);

    auto *vertexLib = reinterpret_cast<MTL::Library*>(shaders[0].ShaderObject);
    auto *fragmentLib = reinterpret_cast<MTL::Library*>(shaders[1].ShaderObject);
    const MTL::Function* vertexFunction = vertexLib->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding));
    const MTL::Function* fragmentFunction = fragmentLib->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding));
    desc->setVertexFunction(vertexFunction);
    desc->setFragmentFunction(fragmentFunction);

    if (!name.empty())
    {
        desc->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    auto *vertDesc = desc->vertexDescriptor();

    for (const auto & attr : vertexAttributes)
    {
        auto *attrDesc = vertDesc->attributes()->object(attr.Index);
        attrDesc->setFormat(MetalHelpers::ToVertexFormat(attr.DataType, attr.Dimensions, attr.IsNormalized));
        attrDesc->setBufferIndex(s_MaxBuffers - 1);
        attrDesc->setOffset(attr.Offset);
    }

    auto *layoutDesc = vertDesc->layouts()->object(s_MaxBuffers - 1);
    layoutDesc->setStride(vertexAttributes[0].Stride);
    layoutDesc->setStepRate(1);
    layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);

    MTL::PipelineOption options = MTL::PipelineOptionNone;
    auto *pso = m_Device->newRenderPipelineState(desc, options, nullptr, &s_Error);

    if (s_Error != nullptr)
    {
        throw std::runtime_error(s_Error->localizedDescription()->utf8String());
    }

    GraphicsBackendProgram program{};
    program.Program = reinterpret_cast<uint64_t>(pso);
    return program;
}

void GraphicsBackendMetal::DeleteShader(GraphicsBackendShaderObject shader)
{
    reinterpret_cast<MTL::Library*>(shader.ShaderObject)->release();
}

void GraphicsBackendMetal::DeleteProgram(GraphicsBackendProgram program)
{
    reinterpret_cast<MTL::RenderPipelineState*>(program.Program)->release();
}

bool GraphicsBackendMetal::RequireStrictPSODescriptor()
{
    return true;
}

void GraphicsBackendMetal::UseProgram(GraphicsBackendProgram program)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto pso = reinterpret_cast<MTL::RenderPipelineState*>(program.Program);
    m_RenderCommandEncoder->setRenderPipelineState(pso);
}

void GraphicsBackendMetal::SetClearColor(float r, float g, float b, float a)
{
    auto desc = m_RenderPassDescriptor->colorAttachments()->object(0);
    desc->setClearColor(MTL::ClearColor::Make(r, g, b, a));
}

void GraphicsBackendMetal::SetClearDepth(double depth)
{
    auto desc = m_RenderPassDescriptor->depthAttachment();
    desc->setClearDepth(depth);
}

void GraphicsBackendMetal::DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    assert(m_RenderCommandEncoder != nullptr);

    const BufferData* vertexBufferData = reinterpret_cast<BufferData*>(geometry.VertexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, s_MaxBuffers - 1);
    m_RenderCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendMetal::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    assert(m_RenderCommandEncoder != nullptr);

    const BufferData* vertexBufferData = reinterpret_cast<BufferData*>(geometry.VertexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, s_MaxBuffers - 1);
    m_RenderCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, indicesCount);
}

void GraphicsBackendMetal::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    assert(m_RenderCommandEncoder != nullptr);

    const BufferData* vertexBufferData = reinterpret_cast<BufferData*>(geometry.VertexBuffer.Buffer);
    const BufferData* indexBufferData = reinterpret_cast<BufferData*>(geometry.IndexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, s_MaxBuffers - 1);
    m_RenderCommandEncoder->drawIndexedPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), indexBufferData->Buffer, 0);
}

void GraphicsBackendMetal::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    assert(m_RenderCommandEncoder != nullptr);

    const BufferData* vertexBufferData = reinterpret_cast<BufferData*>(geometry.VertexBuffer.Buffer);
    const BufferData* indexBufferData = reinterpret_cast<BufferData*>(geometry.IndexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, s_MaxBuffers - 1);
    m_RenderCommandEncoder->drawIndexedPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), indexBufferData->Buffer, 0, instanceCount);
}

void GraphicsBackendMetal::CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
    assert(m_BlitCommandEncoder != nullptr);

    auto metalSource = reinterpret_cast<MTL::Texture*>(source.Texture);
    auto metalDestination = GetTextureFromDescriptor(destinationDescriptor);

    if (!metalSource || !metalDestination)
        return;

    MTL::Origin sourceOrigin(sourceX, sourceY, 0);
    MTL::Origin destinationOrigin(destinationX, destinationY, 0);
    MTL::Size size(width, height, 1);
    m_BlitCommandEncoder->copyFromTexture(metalSource, 0, 0, sourceOrigin, size, metalDestination, 0, 0, destinationOrigin);
}

void GraphicsBackendMetal::PushDebugGroup(const std::string& name)
{
    assert(m_RenderCommandEncoder != nullptr);
    m_RenderCommandEncoder->pushDebugGroup(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
}

void GraphicsBackendMetal::PopDebugGroup()
{
    assert(m_RenderCommandEncoder != nullptr);
    m_RenderCommandEncoder->popDebugGroup();
}

void GraphicsBackendMetal::BeginRenderPass(const std::string& name)
{
    assert(m_RenderCommandBuffer != nullptr);
    assert(m_RenderCommandEncoder == nullptr);
    m_RenderCommandEncoder = m_RenderCommandBuffer->renderCommandEncoder(m_RenderPassDescriptor);

    if (!name.empty())
    {
        m_RenderCommandEncoder->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }
}

void GraphicsBackendMetal::EndRenderPass()
{
    assert(m_RenderCommandEncoder != nullptr);

    m_RenderCommandEncoder->endEncoding();
    m_RenderCommandEncoder = nullptr;

    auto colorTargetCount = m_RenderPassDescriptor->colorAttachments()->retainCount();
    for (int i = 0; i < colorTargetCount; ++i)
    {
        m_RenderPassDescriptor->colorAttachments()->setObject(m_BackbufferDescriptor->colorAttachments()->object(i), i);
    }

    m_RenderPassDescriptor->setDepthAttachment(m_BackbufferDescriptor->depthAttachment());
    m_RenderPassDescriptor->setStencilAttachment(m_BackbufferDescriptor->stencilAttachment());
}

void GraphicsBackendMetal::BeginCopyPass(const std::string& name)
{
    assert(m_CopyCommandBuffer != nullptr);
    assert(m_BlitCommandEncoder == nullptr);
    m_BlitCommandEncoder = m_CopyCommandBuffer->blitCommandEncoder();

    if (!name.empty())
    {
        m_BlitCommandEncoder->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }
}

void GraphicsBackendMetal::EndCopyPass()
{
    assert(m_BlitCommandEncoder != nullptr);
    m_BlitCommandEncoder->endEncoding();
    m_BlitCommandEncoder = nullptr;
}

GraphicsBackendDepthStencilState GraphicsBackendMetal::CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction, const std::string& name)
{
    auto descriptor = MTL::DepthStencilDescriptor::alloc()->init();
    descriptor->setDepthWriteEnabled(depthWrite);
    descriptor->setDepthCompareFunction(MetalHelpers::ToDepthCompareFunction(depthFunction));

    if (!name.empty())
    {
        descriptor->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    auto metalState = m_Device->newDepthStencilState(descriptor);
    descriptor->release();

    GraphicsBackendDepthStencilState state{};
    state.m_State = reinterpret_cast<uint64_t>(metalState);
    return state;
}

void GraphicsBackendMetal::DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
    auto metalState = reinterpret_cast<MTL::DepthStencilState*>(state.m_State);
    if (metalState)
    {
        metalState->release();
    }
}

void GraphicsBackendMetal::SetDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto metalState = reinterpret_cast<MTL::DepthStencilState*>(state.m_State);
    if (metalState)
    {
        m_RenderCommandEncoder->setDepthStencilState(metalState);
    }
}

GraphicsBackendFence GraphicsBackendMetal::CreateFence(FenceType fenceType, const std::string& name)
{
    MTL::Event* metalEvent = m_Device->newEvent();

    if (!name.empty())
    {
        metalEvent->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    GraphicsBackendFence fence{};
    fence.Fence = reinterpret_cast<uint64_t>(metalEvent);
    fence.Type = fenceType;
    return fence;
}

void GraphicsBackendMetal::SignalFence(const GraphicsBackendFence& fence)
{
    const MTL::Event* metalEvent = reinterpret_cast<MTL::Event*>(fence.Fence);

    switch (fence.Type)
    {
        case FenceType::RENDER_TO_COPY:
            m_RenderCommandBuffer->encodeSignalEvent(metalEvent, 1);
            break;
        case FenceType::COPY_TO_RENDER:
            m_CopyCommandBuffer->encodeSignalEvent(metalEvent, 1);
            break;
    }
}

void GraphicsBackendMetal::WaitForFence(const GraphicsBackendFence& fence)
{
    const MTL::Event* metalEvent = reinterpret_cast<MTL::Event*>(fence.Fence);

    switch (fence.Type)
    {
        case FenceType::RENDER_TO_COPY:
            m_CopyCommandBuffer->encodeWait(metalEvent, 1);
            break;
        case FenceType::COPY_TO_RENDER:
            m_RenderCommandBuffer->encodeWait(metalEvent, 1);
            break;
    }
}

MTL::Texture* GraphicsBackendMetal::GetTextureFromDescriptor(const GraphicsBackendRenderTargetDescriptor& descriptor)
{
    auto metalTexture = descriptor.Texture.Texture != 0 ? reinterpret_cast<MTL::Texture*>(descriptor.Texture.Texture) : nullptr;
    if (!descriptor.IsBackbuffer)
        return metalTexture;

    if (descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT || descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)
        return m_BackbufferDescriptor->depthAttachment()->texture();

    if (descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT)
        return m_BackbufferDescriptor->stencilAttachment()->texture();

    int index = static_cast<int>(descriptor.Attachment);
    return m_BackbufferDescriptor->colorAttachments()->object(index)->texture();
}

void GraphicsBackendMetal::SetCommandBuffers(class MTL::CommandBuffer* renderCommandBuffer, class MTL::CommandBuffer* copyCommandBuffer)
{
    assert(renderCommandBuffer != nullptr);
    assert(copyCommandBuffer != nullptr);

    m_RenderCommandBuffer = renderCommandBuffer;
    m_CopyCommandBuffer = copyCommandBuffer;

    m_RenderCommandBuffer->setLabel(NS::String::string("Render Command Buffer", NS::UTF8StringEncoding));
    m_CopyCommandBuffer->setLabel(NS::String::string("Copy Command Buffer", NS::UTF8StringEncoding));
}

#endif // RENDER_BACKEND_METAL
