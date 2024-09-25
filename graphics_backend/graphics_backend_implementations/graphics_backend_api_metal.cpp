#if RENDER_ENGINE_APPLE

#include "graphics_backend_api_metal.h"
#include "enums/texture_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/framebuffer_attachment.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_depth_stencil_state.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "helpers/metal_helpers.h"
#include "debug.h"

#include "Metal/Metal.hpp"

NS::Error *s_Error;

MTL::Buffer* argumentBuffer;
MTL::Buffer* argumentBuffer2;
MTL::ArgumentEncoder* encoder;
MTL::Buffer* buffer3;

struct IRDescriptorTableEntry
{
    uint64_t gpuVA;
    uint64_t textureViewID;
    uint64_t metadata;
};

void GraphicsBackendMetal::Init(void *device)
{
    m_Device = reinterpret_cast<MTL::Device*>(device);
    m_RenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    MTL::ArgumentBuffersTier tier = m_Device->argumentBuffersSupport();
    Debug::LogErrorFormat("%1%", {std::to_string(static_cast<int>(tier))});

    argumentBuffer = m_Device->newBuffer(sizeof(IRDescriptorTableEntry) * 2, MTL::ResourceStorageModeManaged);
    argumentBuffer2 = m_Device->newBuffer(sizeof(uint64_t) * 2, MTL::ResourceStorageModeShared);
}

const std::string &GraphicsBackendMetal::GetShadingLanguageDirective()
{
    static const std::string directives = "#include <metal_stdlib>\nusing namespace metal;\n#define METAL_SHADER\n";
    return directives;
}

GraphicsBackendName GraphicsBackendMetal::GetName()
{
    return GraphicsBackendName::METAL;
}

void GraphicsBackendMetal::PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor)
{
    m_CommandBuffer = reinterpret_cast<MTL::CommandBuffer*>(commandBufferPtr);
    m_BackbufferDescriptor = reinterpret_cast<MTL::RenderPassDescriptor*>(backbufferDescriptor);
}

GraphicsBackendTexture GraphicsBackendMetal::CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels, bool isRenderTarget)
{
    auto storageMode = isRenderTarget ? MTL::StorageModePrivate : MTL::StorageModeManaged;

    MTL::TextureUsage textureUsage = MTL::TextureUsageShaderRead;
    if (isRenderTarget)
    {
        textureUsage |= MTL::TextureUsageRenderTarget;
    }

    auto descriptor = MTL::TextureDescriptor::alloc()->init();
    descriptor->setWidth(width);
    descriptor->setHeight(height);
    descriptor->setPixelFormat(MetalHelpers::ToTextureInternalFormat(format));
    descriptor->setTextureType(MetalHelpers::ToTextureType(type));
    descriptor->setStorageMode(storageMode);
    descriptor->setUsage(textureUsage);
    descriptor->setMipmapLevelCount(mipLevels);

    auto metalTexture = m_Device->newTexture(descriptor);
    descriptor->release();

    GraphicsBackendTexture texture{};
    texture.Texture = reinterpret_cast<uint64_t>(metalTexture);
    texture.Type = type;
    texture.Format = format;
    return texture;
}

GraphicsBackendSampler GraphicsBackendMetal::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod)
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
    assert(m_CurrentCommandEncoder != nullptr);

    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);

    if (bindings.VertexIndex >= 0)
    {
        m_CurrentCommandEncoder->setVertexTexture(metalTexture, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_CurrentCommandEncoder->setFragmentTexture(metalTexture, bindings.FragmentIndex);
    }
}

void GraphicsBackendMetal::BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler)
{
    assert(m_CurrentCommandEncoder != nullptr);

    auto metalSampler = reinterpret_cast<MTL::SamplerState *>(sampler.Sampler);

    if (bindings.VertexIndex >= 0)
    {
        m_CurrentCommandEncoder->setVertexSamplerState(metalSampler, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_CurrentCommandEncoder->setFragmentSamplerState(metalSampler, bindings.FragmentIndex);
    }
}

void GraphicsBackendMetal::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
}

void GraphicsBackendMetal::UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    auto metalTexture = reinterpret_cast<MTL::Texture*>(texture.Texture);
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
    metalTexture->replaceRegion(MTL::Region::Make3D(0, 0, depth, width, height, 1), level, slice, pixelsData, bytesPerRow, 0);
}

void GraphicsBackendMetal::DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels)
{
}

TextureInternalFormat GraphicsBackendMetal::GetTextureFormat(const GraphicsBackendTexture &texture)
{
    return TextureInternalFormat::SRGB_ALPHA;
}

int GraphicsBackendMetal::GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice)
{
    return 0;
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

    if (descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT || descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)
    {
        configureDescriptor(m_RenderPassDescriptor->depthAttachment());
    }
    else if (descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT)
    {
        configureDescriptor(m_RenderPassDescriptor->stencilAttachment());
    }
    else
    {
        int index = static_cast<int>(descriptor.Attachment);
        configureDescriptor(m_RenderPassDescriptor->colorAttachments()->object(index));
    }
}

TextureInternalFormat GraphicsBackendMetal::GetRenderTargetFormat(FramebufferAttachment attachment)
{
    bool isDepth = attachment == FramebufferAttachment::DEPTH_ATTACHMENT || attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
    bool isStencil = attachment == FramebufferAttachment::STENCIL_ATTACHMENT || attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;

    if (isDepth)
    {
        return MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->depthAttachment()->texture()->pixelFormat());
    }
    else if (isStencil)
    {
        return MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->stencilAttachment()->texture()->pixelFormat());
    }
    else
    {
        int index = static_cast<int>(attachment);
        return MetalHelpers::FromTextureInternalFormat(m_RenderPassDescriptor->colorAttachments()->object(index)->texture()->pixelFormat());
    }
}

GraphicsBackendBuffer GraphicsBackendMetal::CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint)
{
    auto metalBuffer = m_Device->newBuffer(size, MTL::ResourceStorageModeManaged);
    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(metalBuffer);
    buffer.IsDataInitialized = true;
    buffer.UsageHint = usageHint;
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendMetal::DeleteBuffer(const GraphicsBackendBuffer &buffer)
{
    auto *metalBuffer = reinterpret_cast<MTL::Buffer*>(buffer.Buffer);
    metalBuffer->release();
}

void GraphicsBackendMetal::BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    assert(m_CurrentCommandEncoder != nullptr);

    auto *metalBuffer = reinterpret_cast<MTL::Buffer*>(buffer.Buffer);
    if (bindings.VertexIndex >= 0)
    {
        m_CurrentCommandEncoder->setVertexBuffer(metalBuffer, offset, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_CurrentCommandEncoder->setFragmentBuffer(metalBuffer, offset, bindings.FragmentIndex);
    }

    //encoder->setBuffer(metalBuffer, 0, bindings.VertexIndex);
    m_CurrentCommandEncoder->useResource(metalBuffer, MTL::ResourceUsageRead, MTL::RenderStageVertex);

//    auto content = reinterpret_cast<IRDescriptorTableEntry*>(argumentBuffer->contents()) + bindings.VertexIndex;
//    content->gpuVA = metalBuffer->gpuAddress();
//    content->textureViewID = 0;
//    content->metadata = 0;
//    argumentBuffer->didModifyRange(NS::Range::Make(0, sizeof(IRDescriptorTableEntry) * 2));

    auto content2 = reinterpret_cast<uint64_t *>(argumentBuffer2->contents()) + bindings.VertexIndex;
    uint64_t test = metalBuffer->gpuAddress();
    Debug::LogErrorFormat("%1%", {std::to_string(test)});
    *content2 = metalBuffer->gpuAddress();
    //argumentBuffer2->didModifyRange(NS::Range::Make(0, sizeof(uint64_t ) * 2));

    //encoder->setBuffer(metalBuffer, 0, bindings.VertexIndex);
}

void GraphicsBackendMetal::SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data)
{
    auto metalBuffer = reinterpret_cast<MTL::Buffer *>(buffer.Buffer);
    auto contents = reinterpret_cast<uint8_t*>(metalBuffer->contents()) + offset;
    memcpy(contents, data, size);
    metalBuffer->didModifyRange(NS::Range::Make(offset, size));
}

void GraphicsBackendMetal::CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size)
{
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

GraphicsBackendGeometry GraphicsBackendMetal::CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes)
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

void GraphicsBackendMetal::EnableVertexAttributeArray(int index)
{
}

void GraphicsBackendMetal::DisableVertexAttributeArray(int index)
{
}

void GraphicsBackendMetal::SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer)
{
}

void GraphicsBackendMetal::SetVertexAttributeDivisor(int index, int divisor)
{
}

void GraphicsBackendMetal::SetCullFace(CullFace cullFace)
{
    assert(m_CurrentCommandEncoder != nullptr);
    m_CurrentCommandEncoder->setCullMode(MetalHelpers::ToCullFace(cullFace));
}

void GraphicsBackendMetal::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    assert(m_CurrentCommandEncoder != nullptr);
    m_CurrentCommandEncoder->setFrontFacingWinding(MetalHelpers::ToCullFaceOrientation(orientation));
}

void GraphicsBackendMetal::SetViewport(int x, int y, int width, int height, float near, float far)
{
    assert(m_CurrentCommandEncoder != nullptr);

    MTL::Viewport viewport {static_cast<double>(x), static_cast<double>(y), static_cast<double>(width), static_cast<double>(height), near, far} ;
    m_CurrentCommandEncoder->setViewport(viewport);
}

GraphicsBackendShaderObject GraphicsBackendMetal::CompileShader(ShaderType shaderType, const std::string &source)
{
    auto library = m_Device->newLibrary(NS::String::string(source.c_str(), NS::UTF8StringEncoding), nullptr, &s_Error);
    if (!library)
    {
        throw std::runtime_error(s_Error->localizedDescription()->utf8String());
    }

    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = reinterpret_cast<uint64_t>(library);
    return shaderObject;
}

GraphicsBackendShaderObject GraphicsBackendMetal::CompileShader2(ShaderType shaderType, const std::vector<uint8_t>& binary)
{
    dispatch_data_t data = dispatch_data_create(binary.data(), binary.size(), dispatch_get_main_queue(), nullptr);
    auto library = m_Device->newLibrary(data, &s_Error);
    if (!library)
    {
        throw std::runtime_error(s_Error->localizedDescription()->utf8String());
    }

    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = reinterpret_cast<uint64_t>(library);
    return shaderObject;
}

GraphicsBackendProgram GraphicsBackendMetal::CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat,
                                                           const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                                                           std::unordered_map<std::string, GraphicsBackendTextureInfo>* textures, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>* buffers)
{
    MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();

    MTL::PixelFormat metalColorFormat = MetalHelpers::ToTextureInternalFormat(colorAttachmentDescriptor.Format);
    MTL::PixelFormat metalDepthFormat = MetalHelpers::ToTextureInternalFormat(depthFormat);

    auto attachmentDesc = desc->colorAttachments()->object(0);
    attachmentDesc->setPixelFormat(metalColorFormat);
    attachmentDesc->setBlendingEnabled(colorAttachmentDescriptor.BlendingEnabled);
    attachmentDesc->setSourceRGBBlendFactor(MetalHelpers::ToBlendFactor(colorAttachmentDescriptor.SourceFactor));
    attachmentDesc->setDestinationRGBBlendFactor(MetalHelpers::ToBlendFactor(colorAttachmentDescriptor.DestinationFactor));

    desc->setDepthAttachmentPixelFormat(metalDepthFormat);

    if (shaders.size() == 1)
    {
    auto shader = shaders[0];
    auto *library = reinterpret_cast<MTL::Library *>(shader.ShaderObject);
    desc->setVertexFunction(library->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding)));
    desc->setFragmentFunction(library->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding)));
    }
    else
    {
        auto shader1 = shaders[0];
        auto shader2 = shaders[1];
        auto *library1 = reinterpret_cast<MTL::Library *>(shader1.ShaderObject);
        auto *library2 = reinterpret_cast<MTL::Library *>(shader2.ShaderObject);
        auto vertexFunction = library1->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding));
        auto fragmentFunction = library2->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding));
        desc->setVertexFunction(vertexFunction);
        desc->setFragmentFunction(fragmentFunction);

//        encoder = vertexFunction->newArgumentEncoder(2);
//        buffer3 = m_Device->newBuffer(encoder->encodedLength(), 0);
//        encoder->setArgumentBuffer(buffer3, 0);
    }

    auto *vertDesc = desc->vertexDescriptor();

    for (const auto & attr : vertexAttributes)
    {
        auto *attrDesc = vertDesc->attributes()->object(attr.Index + (shaders.size() > 1 ? 11 : 0));
        //auto *attrDesc = vertDesc->attributes()->object(attr.Index);
        attrDesc->setFormat(MetalHelpers::ToVertexFormat(attr.DataType, attr.Dimensions, attr.IsNormalized));
        attrDesc->setBufferIndex(0);
        attrDesc->setOffset(attr.Offset);
    }

    auto *layoutDesc = vertDesc->layouts()->object(0);
    layoutDesc->setStride(vertexAttributes[0].Stride);
    layoutDesc->setStepRate(1);
    layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);

    MTL::AutoreleasedRenderPipelineReflection reflection;
    MTL::PipelineOption options = textures && buffers ? MTL::PipelineOptionArgumentInfo | MTL::PipelineOptionBufferTypeInfo : MTL::PipelineOptionNone;
    auto *pso = m_Device->newRenderPipelineState(desc, options, &reflection, &s_Error);

    if (s_Error != nullptr)
    {
        throw std::runtime_error(s_Error->localizedDescription()->utf8String());
    }

    IntrospectProgram(reflection, textures, buffers);

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

void SetBindingIndex(GraphicsBackendResourceBindings &bindings, int index, bool isVertexShader)
{
    if (isVertexShader)
    {
        bindings.VertexIndex = index;
    }
    else
    {
        bindings.FragmentIndex = index;
    }
}

void ParseArguments(NS::Array *arguments, std::unordered_map<std::string, GraphicsBackendTextureInfo> &textures, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers, bool isVertexShader)
{
    for (int i = 0; i < arguments->count(); ++i)
    {
        auto argument = reinterpret_cast<MTL::Argument*>(arguments->object(i));

        auto type = argument->type();
        auto name = argument->name()->utf8String();
        auto index = argument->index();

        if (type == MTL::ArgumentType::ArgumentTypeBuffer)
        {
            auto it = buffers.find(name);
            if (it != buffers.end())
            {
                auto bindings = it->second->GetBinding();
                SetBindingIndex(bindings, index, isVertexShader);
                it->second->SetBindings(bindings);
            }
            else
            {
                std::unordered_map<std::string, int> variables;

                auto members = argument->bufferStructType()->members();
                for (int j = 0; j < members->count(); ++j)
                {
                    auto member = reinterpret_cast<MTL::StructMember*>(members->object(j));
                    variables[member->name()->utf8String()] = member->offset();
                }

                auto bufferSize = argument->bufferDataSize();
                auto buffer = std::make_shared<GraphicsBackendBufferInfo>(GraphicsBackendBufferInfo::BufferType::SHADER_STORAGE, bufferSize, variables);

                GraphicsBackendResourceBindings bindings;
                SetBindingIndex(bindings, index, isVertexShader);
                buffer->SetBindings(bindings);

                buffers[name] = buffer;
            }
        }
        else if (type == MTL::ArgumentType::ArgumentTypeTexture)
        {
            auto it = textures.find(name);
            if (it != textures.end())
            {
                SetBindingIndex(it->second.TextureBindings, index, isVertexShader);
            }
            else
            {
                GraphicsBackendTextureInfo textureInfo;
                textureInfo.Type = MetalHelpers::FromTextureDataType(argument->textureDataType(), argument->textureType());
                SetBindingIndex(textureInfo.TextureBindings, index, isVertexShader);

                textures[name] = textureInfo;
            }
        }
        else if (type == MTL::ArgumentType::ArgumentTypeSampler)
        {
            bool textureFound = false;
            for (auto &it : textures)
            {
                auto &info = it.second;
                if (isVertexShader && info.TextureBindings.VertexIndex == index ||
                    !isVertexShader && info.TextureBindings.FragmentIndex == index)
                {
                    info.HasSampler = true;
                    textureFound = true;
                }
            }

            if (!textureFound)
            {
                Debug::LogErrorFormat("Sampler %1% on index %2% is declared before or without a texture", {name, std::to_string(index)});
            }
        }
    }
}

void GraphicsBackendMetal::IntrospectProgram(MTL::RenderPipelineReflection* reflection, std::unordered_map<std::string, GraphicsBackendTextureInfo>* textures, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>* buffers)
{
    if (reflection && textures && buffers)
    {
        ParseArguments(reflection->vertexArguments(), *textures, *buffers, true);
        ParseArguments(reflection->fragmentArguments(), *textures, *buffers, false);
    }
}

bool GraphicsBackendMetal::RequireStrictPSODescriptor()
{
    return true;
}

void GraphicsBackendMetal::UseProgram(GraphicsBackendProgram program)
{
    assert(m_CurrentCommandEncoder != nullptr);

    auto pso = reinterpret_cast<MTL::RenderPipelineState*>(program.Program);
    m_CurrentCommandEncoder->setRenderPipelineState(pso);
    m_CurrentCommandEncoder->setVertexBuffer(argumentBuffer2, 0, 2);
    //m_CurrentCommandEncoder->setVertexBuffer(buffer3, 0, 2);
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
    assert(m_CurrentCommandEncoder != nullptr);

    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendMetal::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    assert(m_CurrentCommandEncoder != nullptr);

    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, indicesCount);
}

void GraphicsBackendMetal::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    assert(m_CurrentCommandEncoder != nullptr);

    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0);
}

void GraphicsBackendMetal::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    assert(m_CurrentCommandEncoder != nullptr);

    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0, instanceCount);
}

bool GraphicsBackendMetal::SupportShaderStorageBuffer()
{
    return true;
}

void GraphicsBackendMetal::CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
    auto metalSource = reinterpret_cast<MTL::Texture*>(source.Texture);
    auto metalDestination = GetTextureFromDescriptor(destinationDescriptor);

    if (!metalSource || !metalDestination)
        return;

    MTL::Origin sourceOrigin(sourceX, sourceY, 0);
    MTL::Origin destinationOrigin(destinationX, destinationY, 0);
    MTL::Size size(width, height, 1);

    auto encoder = m_CommandBuffer->blitCommandEncoder();
    encoder->copyFromTexture(metalSource, 0, 0, sourceOrigin, size, metalDestination, 0, 0, destinationOrigin);
    encoder->endEncoding();
}

void GraphicsBackendMetal::PushDebugGroup(const std::string &name, int id)
{
    assert(m_CurrentCommandEncoder != nullptr);
    m_CurrentCommandEncoder->pushDebugGroup(NS::String::string(name.c_str(), NS::UTF8StringEncoding));
}

void GraphicsBackendMetal::PopDebugGroup()
{
    assert(m_CurrentCommandEncoder != nullptr);
    m_CurrentCommandEncoder->popDebugGroup();
}

void GraphicsBackendMetal::BeginRenderPass()
{
    assert(m_CurrentCommandEncoder == nullptr);
    m_CurrentCommandEncoder = m_CommandBuffer->renderCommandEncoder(m_RenderPassDescriptor);
}

void GraphicsBackendMetal::EndRenderPass()
{
    assert(m_CurrentCommandEncoder != nullptr);

    m_CurrentCommandEncoder->endEncoding();
    m_CurrentCommandEncoder = nullptr;

    auto colorTargetCount = m_RenderPassDescriptor->colorAttachments()->retainCount();
    for (int i = 0; i < colorTargetCount; ++i)
    {
        m_RenderPassDescriptor->colorAttachments()->setObject(m_BackbufferDescriptor->colorAttachments()->object(i), i);
    }

    m_RenderPassDescriptor->setDepthAttachment(m_BackbufferDescriptor->depthAttachment());
    m_RenderPassDescriptor->setStencilAttachment(m_BackbufferDescriptor->stencilAttachment());
}

GraphicsBackendDepthStencilState GraphicsBackendMetal::CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction)
{
    auto descriptor = MTL::DepthStencilDescriptor::alloc()->init();
    descriptor->setDepthWriteEnabled(depthWrite);
    descriptor->setDepthCompareFunction(MetalHelpers::ToDepthCompareFunction(depthFunction));
    auto metalState = m_Device->newDepthStencilState(descriptor);
    descriptor->release();

    GraphicsBackendDepthStencilState state;
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
    assert(m_CurrentCommandEncoder != nullptr);

    auto metalState = reinterpret_cast<MTL::DepthStencilState*>(state.m_State);
    if (metalState)
    {
        m_CurrentCommandEncoder->setDepthStencilState(metalState);
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

#endif
