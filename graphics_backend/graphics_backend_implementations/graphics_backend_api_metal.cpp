#ifdef RENDER_BACKEND_METAL

#include "graphics_backend_api.h"
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
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "types/graphics_backend_program_descriptor.h"
#include "helpers/metal_helpers.h"
#include "debug.h"

#include "Metal/Metal.hpp"
#include "Metal/MTLCounters.hpp"
#include "MetalKit/MetalKit.hpp"

#include <semaphore>

namespace MetalLocal
{
    constexpr int k_MaxBuffers = 31;
    constexpr int k_MaxTimestampSamples = 100;
    constexpr uint32_t k_ConstantBufferIndexOffset = 8;

    MTL::ResourceOptions s_DefaultBufferStorageMode;

    MTL::CounterSampleBuffer *s_CounterSampleBuffers[k_MaxGPUQueuesCount];
    bool s_CounterSampleFinished[k_MaxGPUQueuesCount][k_MaxTimestampSamples];
    int s_CurrentCounterSampleOffsets[k_MaxGPUQueuesCount];

    MTL::Timestamp s_CpuStartTimestamp;
    MTL::Timestamp s_GpuStartTimestamp;

    std::counting_semaphore s_FramesInFlightSemaphore{GraphicsBackend::GetMaxFramesInFlight()};

    struct BufferData {
        MTL::Buffer *Buffer;
        MTL::ResourceOptions StorageMode;
    };

    struct PSOData
    {
        MTL::RenderPipelineState* PSO;
        MTL::DepthStencilState* DepthStencilState;
        MTL::CullMode CullFace;
        MTL::Winding CullFaceOrientation;
    };

    void SetCounterSampleFinished(uint64_t sampleIndex, int queueIndex, bool finished)
    {
        MetalLocal::s_CounterSampleFinished[queueIndex][sampleIndex] = finished;
        MetalLocal::s_CounterSampleFinished[queueIndex][sampleIndex + 1] = finished;
    }
}

void GraphicsBackendMetal::Init(void *data)
{
    GraphicsBackendBase::Init(data);

    m_View = static_cast<MTK::View*>(data);
    m_Device = m_View->device();

    m_RenderCommandQueue = m_Device->newCommandQueue();
    m_CopyCommandQueue = m_Device->newCommandQueue();

    m_RenderCommandQueue->setLabel(NS::String::string("Render Command Queue", NS::UTF8StringEncoding));
    m_CopyCommandQueue->setLabel(NS::String::string("Copy Command Queue", NS::UTF8StringEncoding));

    m_RenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    SetCommandBuffers(m_RenderCommandQueue->commandBuffer(), m_CopyCommandQueue->commandBuffer());

    MetalLocal::s_DefaultBufferStorageMode = m_Device->hasUnifiedMemory() ? MTL::ResourceStorageModeShared : MTL::ResourceStorageModePrivate;

    if (m_Device->supportsCounterSampling(MTL::CounterSamplingPointAtStageBoundary))
    {
        for (int i = 0; i < m_Device->counterSets()->count(); ++i)
        {
            const MTL::CounterSet* set = reinterpret_cast<MTL::CounterSet*>(m_Device->counterSets()->object(i));
            if (set->name()->isEqualToString(MTL::CommonCounterSetTimestamp))
            {
                for (int j = 0; j < set->counters()->count(); ++j)
                {
                    const MTL::Counter* counter = reinterpret_cast<MTL::Counter*>(set->counters()->object(j));
                    if (counter->name()->isEqualToString(MTL::CommonCounterTimestamp))
                    {
                        MTL::CounterSampleBufferDescriptor* descriptor = MTL::CounterSampleBufferDescriptor::alloc()->init();
                        descriptor->setCounterSet(set);
                        descriptor->setStorageMode(MTL::StorageModeShared);
                        descriptor->setSampleCount(MetalLocal::k_MaxTimestampSamples);

                        for (int gpuQueue = 0; gpuQueue < k_MaxGPUQueuesCount; ++gpuQueue)
                            MetalLocal::s_CounterSampleBuffers[gpuQueue] = m_Device->newCounterSampleBuffer(descriptor, nullptr);
                        descriptor->release();

                        m_Device->sampleTimestamps(&MetalLocal::s_CpuStartTimestamp, &MetalLocal::s_GpuStartTimestamp);
                        m_SupportTimestampCounters = true;
                        break;
                    }
                }
            }
        }
    }
}

GraphicsBackendName GraphicsBackendMetal::GetName()
{
    return GraphicsBackendName::METAL;
}

void GraphicsBackendMetal::InitNewFrame()
{
    GraphicsBackendBase::InitNewFrame();

    MetalLocal::s_FramesInFlightSemaphore.acquire();

    m_BackbufferDescriptor = m_View->currentRenderPassDescriptor();
    SetCommandBuffers(m_RenderCommandQueue->commandBuffer(), m_CopyCommandQueue->commandBuffer());
}

void GraphicsBackendMetal::FillImGuiInitData(void* data)
{
    struct ImGuiData
    {
        MTK::View* View;
        MTL::CommandQueue* RenderQueue;
    };

    ImGuiData* imGuiData = static_cast<ImGuiData*>(data);
    imGuiData->View = m_View;
    imGuiData->RenderQueue = m_RenderCommandQueue;
}

void GraphicsBackendMetal::FillImGuiFrameData(void *data)
{
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

GraphicsBackendSampler GraphicsBackendMetal::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, ComparisonFunction comparisonFunction, const std::string& name)
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

    if (comparisonFunction != ComparisonFunction::NONE)
    {
        const MTL::CompareFunction function = MetalHelpers::ToComparisonFunction(comparisonFunction);
        descriptor->setCompareFunction(function);
    }

    if (!name.empty())
        descriptor->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));

    auto metalSampler = m_Device->newSamplerState(descriptor);
    descriptor->release();

    GraphicsBackendSampler sampler{};
    sampler.Sampler = reinterpret_cast<uint64_t>(metalSampler);
    return sampler;
}

void GraphicsBackendMetal::DeleteTexture_Internal(const GraphicsBackendTexture &texture)
{
    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    metalTexture->release();
}

void GraphicsBackendMetal::DeleteSampler_Internal(const GraphicsBackendSampler &sampler)
{
    auto metalSampler = reinterpret_cast<MTL::SamplerState*>(sampler.Sampler);
    metalSampler->release();
}

void GraphicsBackendMetal::BindTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    m_RenderCommandEncoder->setVertexTexture(metalTexture, index);
    m_RenderCommandEncoder->setFragmentTexture(metalTexture, index);
}

void GraphicsBackendMetal::BindSampler_Internal(const GraphicsBackendSampler& sampler, uint32_t index)
{
    assert(m_RenderCommandEncoder != nullptr);

    auto metalSampler = reinterpret_cast<MTL::SamplerState *>(sampler.Sampler);
    m_RenderCommandEncoder->setVertexSamplerState(metalSampler, index);
    m_RenderCommandEncoder->setFragmentSamplerState(metalSampler, index);
}

void GraphicsBackendMetal::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
    assert(encoder != nullptr);

    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
    if (!metalTexture)
        return;

    encoder->generateMipmaps(metalTexture);
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
    MTL::Buffer* tempBuffer = m_Device->newBuffer(pixelsData, imageSize, MetalLocal::s_DefaultBufferStorageMode);
    BeginCopyPass("Upload Texture Data");
    GetBlitCommandEncoder()->copyFromBuffer(tempBuffer, 0, bytesPerRow, imageSize, MTL::Size::Make(width, height, 1), metalTexture, static_cast<NS::UInteger>(cubemapFace), level, MTL::Origin::Make(0, 0, depth));
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
    const MTL::ResourceOptions storageMode = allowCPUWrites ? MetalLocal::s_DefaultBufferStorageMode : MTL::ResourceStorageModePrivate;
    MTL::Buffer* metalBuffer = allowCPUWrites && data ? m_Device->newBuffer(data, size, storageMode) : m_Device->newBuffer(size, storageMode);

    if (!allowCPUWrites)
    {
        MTL::Buffer* tempBuffer = m_Device->newBuffer(data, size, MetalLocal::s_DefaultBufferStorageMode);
        BeginCopyPass("Init Buffer " + name);
        GetBlitCommandEncoder()->copyFromBuffer(tempBuffer, 0, metalBuffer, 0, size);
        EndCopyPass();

        tempBuffer->release();
    }

    if (!name.empty())
    {
        metalBuffer->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    MetalLocal::BufferData* bufferData = new MetalLocal::BufferData();
    bufferData->Buffer = metalBuffer;
    bufferData->StorageMode = storageMode;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(bufferData);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendMetal::DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer)
{
    const MetalLocal::BufferData* bufferData = reinterpret_cast<MetalLocal::BufferData*>(buffer.Buffer);
    bufferData->Buffer->release();
    delete bufferData;
}

void GraphicsBackendMetal::BindBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    assert(m_RenderCommandEncoder != nullptr);

    const MetalLocal::BufferData* bufferData = reinterpret_cast<MetalLocal::BufferData*>(buffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(bufferData->Buffer, offset, index);
    m_RenderCommandEncoder->setFragmentBuffer(bufferData->Buffer, offset, index);
}

void GraphicsBackendMetal::BindStructuredBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int count)
{
    BindBuffer_Internal(buffer, index, offset, size);
}

void GraphicsBackendMetal::BindConstantBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
{
    BindBuffer_Internal(buffer, index + MetalLocal::k_ConstantBufferIndexOffset, offset, size);
}

void GraphicsBackendMetal::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data)
{
    const MetalLocal::BufferData* bufferData = reinterpret_cast<MetalLocal::BufferData*>(buffer.Buffer);
    uint8_t* contents = static_cast<uint8_t*>(bufferData->Buffer->contents()) + offset;
    memcpy(contents, data, size);

    if (bufferData->StorageMode == MTL::ResourceStorageModeManaged)
        bufferData->Buffer->didModifyRange(NS::Range::Make(offset, size));
}

void GraphicsBackendMetal::CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size)
{
    MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
    assert(encoder != nullptr);

    const MetalLocal::BufferData* sourceBufferData = reinterpret_cast<MetalLocal::BufferData*>(source.Buffer);
    const MetalLocal::BufferData* destinationBufferData = reinterpret_cast<MetalLocal::BufferData*>(destination.Buffer);
    encoder->copyFromBuffer(sourceBufferData->Buffer, sourceOffset, destinationBufferData->Buffer, destinationOffset, size);
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

void GraphicsBackendMetal::DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry)
{
    DeleteBuffer(geometry.VertexBuffer);
    DeleteBuffer(geometry.IndexBuffer);
}

void GraphicsBackendMetal::SetViewport(int x, int y, int width, int height, float near, float far)
{
    assert(m_RenderCommandEncoder != nullptr);

    MTL::Viewport viewport {static_cast<double>(x), static_cast<double>(y), static_cast<double>(width), static_cast<double>(height), near, far} ;
    m_RenderCommandEncoder->setViewport(viewport);
}

void GraphicsBackendMetal::SetScissorRect(int x, int y, int width, int height)
{
}

GraphicsBackendShaderObject GraphicsBackendMetal::CompileShader(ShaderType shaderType, const std::string &source, const std::string& name)
{
    NS::Error* error;
    auto library = m_Device->newLibrary(NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr, &error);
    if (!library)
    {
        throw std::runtime_error(error->localizedDescription()->utf8String());
    }

    if (!name.empty())
    {
        library->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = reinterpret_cast<uint64_t>(library);
    return shaderObject;
}

GraphicsBackendShaderObject GraphicsBackendMetal::CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name)
{
}

GraphicsBackendProgram GraphicsBackendMetal::CreateProgram(const GraphicsBackendProgramDescriptor& descriptor)
{
    assert(descriptor.Shaders->size() == 2);

    MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();

    MTL::PixelFormat metalColorFormat = MetalHelpers::ToTextureInternalFormat(descriptor.ColorAttachmentDescriptor.Format, descriptor.ColorAttachmentDescriptor.IsLinear);
    MTL::PixelFormat metalDepthFormat = MetalHelpers::ToTextureInternalFormat(descriptor.DepthFormat, false);

    auto attachmentDesc = desc->colorAttachments()->object(0);
    attachmentDesc->setPixelFormat(metalColorFormat);
    attachmentDesc->setBlendingEnabled(descriptor.ColorAttachmentDescriptor.BlendingEnabled);
    attachmentDesc->setSourceRGBBlendFactor(MetalHelpers::ToBlendFactor(descriptor.ColorAttachmentDescriptor.SourceFactor));
    attachmentDesc->setDestinationRGBBlendFactor(MetalHelpers::ToBlendFactor(descriptor.ColorAttachmentDescriptor.DestinationFactor));

    desc->setDepthAttachmentPixelFormat(metalDepthFormat);

    if (descriptor.DepthFormat == TextureInternalFormat::DEPTH_32_STENCIL_8 || descriptor.DepthFormat == TextureInternalFormat::DEPTH_24_STENCIL_8)
        desc->setStencilAttachmentPixelFormat(metalDepthFormat);

    const std::vector<GraphicsBackendShaderObject>& shaders = *descriptor.Shaders;
    auto *vertexLib = reinterpret_cast<MTL::Library*>(shaders[0].ShaderObject);
    auto *fragmentLib = reinterpret_cast<MTL::Library*>(shaders[1].ShaderObject);
    const MTL::Function* vertexFunction = vertexLib->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding));
    const MTL::Function* fragmentFunction = fragmentLib->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding));
    desc->setVertexFunction(vertexFunction);
    desc->setFragmentFunction(fragmentFunction);

    if (descriptor.Name && !descriptor.Name->empty())
    {
        desc->setLabel(NS::String::string(descriptor.Name->c_str(), NS::UTF8StringEncoding));
    }

    auto *vertDesc = desc->vertexDescriptor();

    const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes = *descriptor.VertexAttributes;
    for (int i = 0; i < vertexAttributes.size(); ++i)
    {
        const GraphicsBackendVertexAttributeDescriptor& attr = vertexAttributes[i];
        auto *attrDesc = vertDesc->attributes()->object(i);
        attrDesc->setFormat(MetalHelpers::ToVertexFormat(attr.DataType, attr.Dimensions, attr.IsNormalized));
        attrDesc->setBufferIndex(MetalLocal::k_MaxBuffers - 1);
        attrDesc->setOffset(attr.Offset);
    }

    auto *layoutDesc = vertDesc->layouts()->object(MetalLocal::k_MaxBuffers - 1);
    layoutDesc->setStride(vertexAttributes[0].Stride);
    layoutDesc->setStepRate(1);
    layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);

    NS::Error* error;
    MTL::PipelineOption options = MTL::PipelineOptionNone;
    auto *pso = m_Device->newRenderPipelineState(desc, options, nullptr, &error);

    if (error != nullptr)
    {
        Debug::LogError(error->localizedDescription()->utf8String());
        throw std::runtime_error(error->localizedDescription()->utf8String());
    }

    auto depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
    depthStencilDescriptor->setDepthWriteEnabled(descriptor.DepthWrite);
    depthStencilDescriptor->setDepthCompareFunction(MetalHelpers::ToComparisonFunction(descriptor.DepthComparisonFunction));

    if (descriptor.Name && !descriptor.Name->empty())
    {
        std::string depthStencilStateName = *descriptor.Name + "_DepthStencil";
        depthStencilDescriptor->setLabel(NS::String::string(depthStencilStateName.c_str(), NS::UTF8StringEncoding));
    }

    MTL::DepthStencilState* depthStencilState = m_Device->newDepthStencilState(depthStencilDescriptor);
    depthStencilDescriptor->release();

    MetalLocal::PSOData* psoData = new MetalLocal::PSOData();
    psoData->PSO = pso;
    psoData->DepthStencilState = depthStencilState;
    psoData->CullFace = MetalHelpers::ToCullFace(descriptor.CullFace);
    psoData->CullFaceOrientation = MetalHelpers::ToCullFaceOrientation(descriptor.CullFaceOrientation);

    return GraphicsBackendBase::CreateProgram(reinterpret_cast<uint64_t>(psoData), descriptor);
}

void GraphicsBackendMetal::DeleteShader_Internal(GraphicsBackendShaderObject shader)
{
    reinterpret_cast<MTL::Library*>(shader.ShaderObject)->release();
}

void GraphicsBackendMetal::DeleteProgram_Internal(GraphicsBackendProgram program)
{
    MetalLocal::PSOData* psoData = reinterpret_cast<MetalLocal::PSOData*>(program.Program);
    psoData->PSO->release();
    psoData->DepthStencilState->release();
    delete psoData;
}

bool GraphicsBackendMetal::RequireStrictPSODescriptor()
{
    return true;
}

void GraphicsBackendMetal::UseProgram(const GraphicsBackendProgram& program)
{
    assert(m_RenderCommandEncoder != nullptr);

    MetalLocal::PSOData* psoData = reinterpret_cast<MetalLocal::PSOData*>(program.Program);
    m_RenderCommandEncoder->setRenderPipelineState(psoData->PSO);
    m_RenderCommandEncoder->setDepthStencilState(psoData->DepthStencilState);
    m_RenderCommandEncoder->setCullMode(psoData->CullFace);
    m_RenderCommandEncoder->setFrontFacingWinding(psoData->CullFaceOrientation);

    BindResources(program);
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

    const MetalLocal::BufferData* vertexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.VertexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, MetalLocal::k_MaxBuffers - 1);
    m_RenderCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendMetal::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    assert(m_RenderCommandEncoder != nullptr);

    const MetalLocal::BufferData* vertexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.VertexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, MetalLocal::k_MaxBuffers - 1);
    m_RenderCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, indicesCount);
}

void GraphicsBackendMetal::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    assert(m_RenderCommandEncoder != nullptr);

    const MetalLocal::BufferData* vertexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.VertexBuffer.Buffer);
    const MetalLocal::BufferData* indexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.IndexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, MetalLocal::k_MaxBuffers - 1);
    m_RenderCommandEncoder->drawIndexedPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), indexBufferData->Buffer, 0);
}

void GraphicsBackendMetal::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    assert(m_RenderCommandEncoder != nullptr);

    const MetalLocal::BufferData* vertexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.VertexBuffer.Buffer);
    const MetalLocal::BufferData* indexBufferData = reinterpret_cast<MetalLocal::BufferData*>(geometry.IndexBuffer.Buffer);
    m_RenderCommandEncoder->setVertexBuffer(vertexBufferData->Buffer, 0, MetalLocal::k_MaxBuffers - 1);
    m_RenderCommandEncoder->drawIndexedPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), indexBufferData->Buffer, 0, instanceCount);
}

void GraphicsBackendMetal::CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
    MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
    assert(encoder != nullptr);

    auto metalSource = reinterpret_cast<MTL::Texture*>(source.Texture);
    auto metalDestination = GetTextureFromDescriptor(destinationDescriptor);

    if (!metalSource || !metalDestination)
        return;

    MTL::Origin sourceOrigin(sourceX, sourceY, 0);
    MTL::Origin destinationOrigin(destinationX, destinationY, 0);
    MTL::Size size(width, height, 1);
    encoder->copyFromTexture(metalSource, 0, 0, sourceOrigin, size, metalDestination, 0, 0, destinationOrigin);
}

void GraphicsBackendMetal::PushDebugGroup(const std::string& name, GPUQueue queue)
{
    switch (queue)
    {
        case GPUQueue::RENDER:
            assert(m_RenderCommandEncoder != nullptr);
            m_RenderCommandEncoder->pushDebugGroup(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
            break;
        case GPUQueue::COPY:
            MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
            assert(encoder != nullptr);
            encoder->pushDebugGroup(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
            break;
    }
}

void GraphicsBackendMetal::PopDebugGroup(GPUQueue queue)
{
    switch (queue)
    {
        case GPUQueue::RENDER:
            assert(m_RenderCommandEncoder != nullptr);
            m_RenderCommandEncoder->popDebugGroup();
            break;
        case GPUQueue::COPY:
            MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
            assert(encoder != nullptr);
            encoder->popDebugGroup();
            break;
    }
}

GraphicsBackendProfilerMarker GraphicsBackendMetal::PushProfilerMarker()
{
    assert(!m_ProfilerMarkerActive);
    m_ProfilerMarkerActive = m_SupportTimestampCounters;

    GraphicsBackendProfilerMarker marker{};
    for (int gpuQueue = 0; gpuQueue < k_MaxGPUQueuesCount; ++gpuQueue)
        marker.Info[gpuQueue].StartMarker = MetalLocal::s_CurrentCounterSampleOffsets[gpuQueue];
    return marker;
}

void GraphicsBackendMetal::PopProfilerMarker(GraphicsBackendProfilerMarker& marker)
{
    assert(m_ProfilerMarkerActive);
    m_ProfilerMarkerActive = false;

    for (int gpuQueue = 0; gpuQueue < k_MaxGPUQueuesCount; ++gpuQueue)
    {
        GraphicsBackendProfilerMarker::MarkerInfo& info = marker.Info[gpuQueue];
        const int currentCounterSampleOffset = MetalLocal::s_CurrentCounterSampleOffsets[gpuQueue];
        const bool isActive = info.StartMarker != currentCounterSampleOffset;
        const uint64_t sampleIndex = currentCounterSampleOffset == 0 ? MetalLocal::k_MaxTimestampSamples - 1 : currentCounterSampleOffset - 1;
        info.EndMarker = isActive ? sampleIndex : info.StartMarker;
    }
}

bool GraphicsBackendMetal::ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResults& outResults)
{
    if (!m_SupportTimestampCounters)
        return true;

    static MTL::Timestamp cpuDuration;
    static MTL::Timestamp gpuDuration;
    static std::uint64_t timestampDifference;
    if (cpuDuration == 0 || gpuDuration == 0)
    {
        MTL::Timestamp cpuTimestamp;
        MTL::Timestamp gpuTimestamp;
        m_Device->sampleTimestamps(&cpuTimestamp, &gpuTimestamp);

        cpuDuration = cpuTimestamp - MetalLocal::s_CpuStartTimestamp;
        gpuDuration = gpuTimestamp - MetalLocal::s_GpuStartTimestamp;

        const uint64_t systemCpuTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        timestampDifference = systemCpuTimestamp - cpuTimestamp;
    }

    auto tryResolve = [](uint64_t counterOffset, int queueIndex, uint64_t& outCpuTimestamp)
    {
        if (!MetalLocal::s_CounterSampleFinished[queueIndex][counterOffset])
            return false;

        const NS::Data* data = MetalLocal::s_CounterSampleBuffers[queueIndex]->resolveCounterRange(NS::Range::Make(counterOffset, 1));
        const MTL::CounterResultTimestamp* timestampData = static_cast<MTL::CounterResultTimestamp*>(data->mutableBytes());
        const uint64_t gpuTimestamp = timestampData->timestamp;

        const double normalizedGpuTime = static_cast<double>(gpuTimestamp - MetalLocal::s_GpuStartTimestamp) / gpuDuration;
        const double nanoseconds = normalizedGpuTime * cpuDuration + MetalLocal::s_CpuStartTimestamp + timestampDifference;
        outCpuTimestamp = nanoseconds / 1000;
        return true;
    };

    bool resolved = true;
    for (int gpuQueue = 0; gpuQueue < k_MaxGPUQueuesCount; ++gpuQueue)
    {
        const GraphicsBackendProfilerMarker::MarkerInfo& markerInfo = marker.Info[gpuQueue];
        outResults[gpuQueue].IsActive = markerInfo.StartMarker != markerInfo.EndMarker;
        if (!outResults[gpuQueue].IsActive)
            continue;

        resolved &= tryResolve(markerInfo.StartMarker, gpuQueue, outResults[gpuQueue].StartTimestamp);
        resolved &= tryResolve(markerInfo.EndMarker, gpuQueue, outResults[gpuQueue].EndTimestamp);
    }

    return resolved;
}

void GraphicsBackendMetal::BeginRenderPass(const std::string& name)
{
    assert(m_RenderCommandBuffer != nullptr);
    assert(m_RenderCommandEncoder == nullptr);

    if (m_ProfilerMarkerActive)
    {
        uint64_t counterSamplerIndex = MetalLocal::s_CurrentCounterSampleOffsets[k_RenderGPUQueueIndex];
        MetalLocal::SetCounterSampleFinished(counterSamplerIndex, k_RenderGPUQueueIndex, false);

        MTL::RenderPassSampleBufferAttachmentDescriptor* descriptor = m_RenderPassDescriptor->sampleBufferAttachments()->object(0);
        descriptor->setSampleBuffer(MetalLocal::s_CounterSampleBuffers[k_RenderGPUQueueIndex]);
        descriptor->setStartOfVertexSampleIndex(counterSamplerIndex);
        descriptor->setEndOfVertexSampleIndex(NS::UIntegerMax);
        descriptor->setStartOfFragmentSampleIndex(NS::UIntegerMax);
        descriptor->setEndOfFragmentSampleIndex(counterSamplerIndex + 1);

        m_RenderCommandBuffer->addCompletedHandler([counterSamplerIndex](class MTL::CommandBuffer*)
        {
            MetalLocal::SetCounterSampleFinished(counterSamplerIndex, k_RenderGPUQueueIndex, true);
        });

        MetalLocal::s_CurrentCounterSampleOffsets[k_RenderGPUQueueIndex] = (MetalLocal::s_CurrentCounterSampleOffsets[k_RenderGPUQueueIndex] + 2) % MetalLocal::k_MaxTimestampSamples;
    }

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

    const NS::UInteger colorTargetCount = m_RenderPassDescriptor->colorAttachments()->retainCount();
    for (int i = 0; i < colorTargetCount; ++i)
    {
        m_RenderPassDescriptor->colorAttachments()->setObject(m_BackbufferDescriptor->colorAttachments()->object(i), i);
    }

    m_RenderPassDescriptor->setDepthAttachment(m_BackbufferDescriptor->depthAttachment());
    m_RenderPassDescriptor->setStencilAttachment(m_BackbufferDescriptor->stencilAttachment());

    m_RenderPassDescriptor->sampleBufferAttachments()->setObject(nullptr, 0);
}

void GraphicsBackendMetal::BeginCopyPass(const std::string& name)
{
    const bool isMainThread = IsMainThread();
    MTL::CommandBuffer* buffer = isMainThread ? m_CopyCommandBuffer : GetOrCreateUploadCommandBuffer();

    assert(buffer != nullptr);
    assert(GetBlitCommandEncoder() == nullptr);

    MTL::BlitPassDescriptor* passDescriptor = MTL::BlitPassDescriptor::alloc()->init();
    if (m_ProfilerMarkerActive && isMainThread)
    {
        uint64_t counterSamplerIndex = MetalLocal::s_CurrentCounterSampleOffsets[k_CopyGPUQueueIndex];
        MetalLocal::SetCounterSampleFinished(counterSamplerIndex, k_CopyGPUQueueIndex, false);

        MTL::BlitPassSampleBufferAttachmentDescriptor* descriptor = passDescriptor->sampleBufferAttachments()->object(0);
        descriptor->setSampleBuffer(MetalLocal::s_CounterSampleBuffers[k_CopyGPUQueueIndex]);
        descriptor->setStartOfEncoderSampleIndex(counterSamplerIndex);
        descriptor->setEndOfEncoderSampleIndex(counterSamplerIndex + 1);

        buffer->addCompletedHandler([counterSamplerIndex](class MTL::CommandBuffer*)
        {
            MetalLocal::SetCounterSampleFinished(counterSamplerIndex, k_CopyGPUQueueIndex, true);
        });

        MetalLocal::s_CurrentCounterSampleOffsets[k_CopyGPUQueueIndex] = (MetalLocal::s_CurrentCounterSampleOffsets[k_CopyGPUQueueIndex] + 2) % MetalLocal::k_MaxTimestampSamples;
    }

    MTL::BlitCommandEncoder* encoder = buffer->blitCommandEncoder(passDescriptor);
    passDescriptor->release();

    if (!name.empty())
    {
        encoder->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
    }

    SetBlitCommandEncoder(encoder);
}

void GraphicsBackendMetal::EndCopyPass()
{
    MTL::BlitCommandEncoder* encoder = GetBlitCommandEncoder();
    assert(encoder != nullptr);
    encoder->endEncoding();
    SetBlitCommandEncoder(nullptr);

    if (!IsMainThread())
    {
        MTL::CommandBuffer* buffer = GetOrCreateUploadCommandBuffer();
        buffer->commit();
        buffer->waitUntilCompleted();
        SetUploadCommandBuffer(nullptr);
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

void GraphicsBackendMetal::DeleteFence(const GraphicsBackendFence& fence)
{
    MTL::Event* metalEvent = reinterpret_cast<MTL::Event*>(fence.Fence);
    metalEvent->release();
}

void GraphicsBackendMetal::SignalFence(const GraphicsBackendFence& fence)
{
    const MTL::Event* metalEvent = reinterpret_cast<MTL::Event*>(fence.Fence);

    switch (fence.Type)
    {
        case FenceType::RENDER_TO_COPY:
            m_RenderCommandBuffer->encodeSignalEvent(metalEvent, GetFrameNumber());
            break;
        case FenceType::COPY_TO_RENDER:
            m_CopyCommandBuffer->encodeSignalEvent(metalEvent, GetFrameNumber());
            break;
    }
}

void GraphicsBackendMetal::WaitForFence(const GraphicsBackendFence& fence)
{
    const MTL::Event* metalEvent = reinterpret_cast<MTL::Event*>(fence.Fence);

    switch (fence.Type)
    {
        case FenceType::RENDER_TO_COPY:
            m_CopyCommandBuffer->encodeWait(metalEvent, GetFrameNumber());
            break;
        case FenceType::COPY_TO_RENDER:
            m_RenderCommandBuffer->encodeWait(metalEvent, GetFrameNumber());
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

MTL::CommandBuffer* GraphicsBackendMetal::GetOrCreateUploadCommandBuffer()
{
    std::thread::id threadId = std::this_thread::get_id();

    std::unique_lock lock(m_UploadCommandBuffersMutex);
    auto it = m_UploadCommandBuffers.find(threadId);
    if (it != m_UploadCommandBuffers.end() && it->second)
        return it->second;

    MTL::CommandBuffer* buffer = m_CopyCommandQueue->commandBuffer();
    m_UploadCommandBuffers[threadId] = buffer;
    return buffer;
}

void GraphicsBackendMetal::SetUploadCommandBuffer(MTL::CommandBuffer* buffer)
{
    std::unique_lock lock(m_UploadCommandBuffersMutex);
    m_UploadCommandBuffers[std::this_thread::get_id()] = buffer;
}

MTL::BlitCommandEncoder* GraphicsBackendMetal::GetBlitCommandEncoder()
{
    std::shared_lock lock(m_BlitCommandEncodersMutex);
    auto it = m_BlitCommandEncoders.find(std::this_thread::get_id());
    return it != m_BlitCommandEncoders.end() ? it->second : nullptr;
}

void GraphicsBackendMetal::SetBlitCommandEncoder(MTL::BlitCommandEncoder* encoder)
{
    std::unique_lock lock(m_BlitCommandEncodersMutex);
    m_BlitCommandEncoders[std::this_thread::get_id()] = encoder;
}

void GraphicsBackendMetal::Present()
{
    m_CopyCommandBuffer->commit();
    m_RenderCommandBuffer->addCompletedHandler([](MTL::CommandBuffer*){MetalLocal::s_FramesInFlightSemaphore.release();});
    m_RenderCommandBuffer->presentDrawable(m_View->currentDrawable());
    m_RenderCommandBuffer->commit();
}

void GraphicsBackendMetal::Flush()
{
    m_RenderCommandBuffer->commit();
    m_CopyCommandBuffer->commit();

    SetCommandBuffers(m_RenderCommandQueue->commandBuffer(), m_CopyCommandQueue->commandBuffer());
}

void GraphicsBackendMetal::TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor, ResourceState state, GPUQueue queue)
{
}

void GraphicsBackendMetal::TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue)
{
}

void GraphicsBackendMetal::TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue)
{
}

#endif // RENDER_BACKEND_METAL
