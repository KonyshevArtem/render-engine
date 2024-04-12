#if RENDER_ENGINE_APPLE

#include "graphics_backend_api_metal.h"
#include "graphics_backend_debug.h"
#include "enums/uniform_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_framebuffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_uniform_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "helpers/metal_helpers.h"
#include "debug.h"

#include "Metal/Metal.hpp"

NS::Error *s_Error;

void GraphicsBackendMetal::Init(void *device)
{
    m_Device = reinterpret_cast<MTL::Device*>(device);
}

int GraphicsBackendMetal::GetMajorVersion()
{
    return 0;
}

int GraphicsBackendMetal::GetMinorVersion()
{
    return 0;
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

GraphicsBackendTexture GraphicsBackendMetal::CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels)
{
    auto descriptor = MTL::TextureDescriptor::alloc()->init();
    descriptor->setWidth(width);
    descriptor->setHeight(height);
    descriptor->setPixelFormat(MetalHelpers::ToTextureInternalFormat(format));
    descriptor->setTextureType(MetalHelpers::ToTextureType(type));
    descriptor->setStorageMode(MTL::StorageModeManaged);
    descriptor->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
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

void GraphicsBackendMetal::BindTexture(const GraphicsBackendResourceBindings &bindings, int uniformLocation, const GraphicsBackendTexture &texture)
{
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

void GraphicsBackendMetal::GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
}

void GraphicsBackendMetal::DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
}

void GraphicsBackendMetal::BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer)
{
}

void GraphicsBackendMetal::SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level)
{
}

void GraphicsBackendMetal::SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer)
{
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

void GraphicsBackendMetal::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
}

void GraphicsBackendMetal::BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    auto *metalBuffer = reinterpret_cast<MTL::Buffer*>(buffer.Buffer);
    if (bindings.VertexIndex >= 0)
    {
        m_CurrentCommandEncoder->setVertexBuffer(metalBuffer, offset, bindings.VertexIndex);
    }
    if (bindings.FragmentIndex >= 0)
    {
        m_CurrentCommandEncoder->setFragmentBuffer(metalBuffer, offset, bindings.FragmentIndex);
    }
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

void GraphicsBackendMetal::SetCapability(GraphicsBackendCapability capability, bool enabled)
{
}

void GraphicsBackendMetal::SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
}

void GraphicsBackendMetal::SetCullFace(CullFace cullFace)
{
}

void GraphicsBackendMetal::SetCullFaceOrientation(CullFaceOrientation orientation)
{
}

void GraphicsBackendMetal::SetDepthFunction(DepthFunction function)
{
}

void GraphicsBackendMetal::SetDepthWrite(bool enabled)
{
}

void GraphicsBackendMetal::SetDepthRange(double near, double far)
{
}

void GraphicsBackendMetal::SetViewport(int x, int y, int width, int height)
{
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

GraphicsBackendProgram GraphicsBackendMetal::CreateProgram(GraphicsBackendShaderObject *shaders, int shadersCount)
{
    MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();
    desc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    desc->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);

    auto shader = shaders[0];
    auto *library = reinterpret_cast<MTL::Library *>(shader.ShaderObject);
    desc->setVertexFunction(library->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding)));
    desc->setFragmentFunction(library->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding)));

    auto *vertDesc = desc->vertexDescriptor();

    auto *attrDesc = vertDesc->attributes()->object(0);
    attrDesc->setFormat(MTL::VertexFormatFloat3);
    attrDesc->setBufferIndex(0);
    attrDesc->setOffset(0);

    auto *layoutDesc = vertDesc->layouts()->object(0);
    layoutDesc->setStride(12);
    layoutDesc->setStepRate(1);
    layoutDesc->setStepFunction(MTL::VertexStepFunctionPerVertex);

    MTL::AutoreleasedRenderPipelineReflection reflection;
    MTL::PipelineOption options = MTL::PipelineOptionArgumentInfo | MTL::PipelineOptionBufferTypeInfo;
    auto *pso = m_Device->newRenderPipelineState(desc, options, &reflection, &s_Error);

    GraphicsBackendProgram program{};
    program.Program = reinterpret_cast<uint64_t>(pso);
    program.Reflection = reinterpret_cast<uint64_t>(reflection);
    return program;
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

void ParseArguments(NS::Array *arguments, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers, bool isVertexShader)
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
            auto it = uniforms.find(name);
            if (it != uniforms.end())
            {
                SetBindingIndex(it->second.TextureBindings, index, isVertexShader);
            }
            else
            {
                GraphicsBackendUniformInfo uniformInfo;
                uniformInfo.IsTexture = true;
                SetBindingIndex(uniformInfo.TextureBindings, index, isVertexShader);

                uniforms[name] = uniformInfo;
            }
        }
        else if (type == MTL::ArgumentType::ArgumentTypeSampler)
        {
            bool textureFound = false;
            for (auto &it : uniforms)
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

void GraphicsBackendMetal::IntrospectProgram(GraphicsBackendProgram program, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers)
{
    auto reflection = reinterpret_cast<MTL::AutoreleasedRenderPipelineReflection>(program.Reflection);
    ParseArguments(reflection->vertexArguments(), uniforms, buffers, true);
    ParseArguments(reflection->fragmentArguments(), uniforms, buffers, false);
}

void GraphicsBackendMetal::UseProgram(GraphicsBackendProgram program)
{
    auto pso = reinterpret_cast<MTL::RenderPipelineState*>(program.Program);
    m_CurrentCommandEncoder->setRenderPipelineState(pso);
}

void GraphicsBackendMetal::SetUniform(int location, UniformDataType dataType, int count, const void *data, bool transpose)
{
}

void GraphicsBackendMetal::Clear(ClearMask mask)
{
}

void GraphicsBackendMetal::SetClearColor(float r, float g, float b, float a)
{
}

void GraphicsBackendMetal::SetClearDepth(double depth)
{
}

void GraphicsBackendMetal::DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendMetal::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawPrimitives(MetalHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, indicesCount);
}

void GraphicsBackendMetal::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0);
}

void GraphicsBackendMetal::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), MetalHelpers::ToIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0, instanceCount);
}

bool GraphicsBackendMetal::SupportShaderStorageBuffer()
{
    return true;
}

void GraphicsBackendMetal::BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter)
{
}

void GraphicsBackendMetal::PushDebugGroup(const std::string &name, int id)
{
}

void GraphicsBackendMetal::PopDebugGroup()
{
}

void GraphicsBackendMetal::BeginRenderPass()
{
    EndRenderPass();
    m_CurrentCommandEncoder = m_CommandBuffer->renderCommandEncoder(m_BackbufferDescriptor);
}

void GraphicsBackendMetal::EndRenderPass()
{
    if (m_CurrentCommandEncoder)
    {
        m_CurrentCommandEncoder->endEncoding();
        m_CurrentCommandEncoder = nullptr;
    }
}

GRAPHICS_BACKEND_TYPE_ENUM GraphicsBackendMetal::GetError()
{
    return 0;
}

const char *GraphicsBackendMetal::GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error)
{
    return nullptr;
}

#endif
