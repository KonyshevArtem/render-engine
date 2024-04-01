#if RENDER_ENGINE_APPLE

#include "graphics_backend_api_metal.h"
#include "graphics_backend_debug.h"
#include "enums/texture_unit.h"
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
#include "debug.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Metal/Metal.hpp"

NS::Error *s_Error;

MTL::PrimitiveType ToMetalPrimitiveType(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case PrimitiveType::POINTS:
            return MTL::PrimitiveType::PrimitiveTypePoint;
        case PrimitiveType::LINE_STRIP:
            return MTL::PrimitiveType::PrimitiveTypeLineStrip;
        case PrimitiveType::LINES:
            return MTL::PrimitiveType::PrimitiveTypeLine;
        case PrimitiveType::TRIANGLE_STRIP:
            return MTL::PrimitiveType::PrimitiveTypeTriangleStrip;
        case PrimitiveType::TRIANGLES:
            return MTL::PrimitiveType::PrimitiveTypeTriangle;
        default:
            return MTL::PrimitiveType::PrimitiveTypeTriangle;
    }
}

MTL::IndexType ToMetalIndicesDataType(IndicesDataType dataType)
{
    switch (dataType)
    {
        case IndicesDataType::UNSIGNED_BYTE:
        case IndicesDataType::UNSIGNED_SHORT:
            return MTL::IndexType::IndexTypeUInt16;
        case IndicesDataType::UNSIGNED_INT:
            return MTL::IndexType::IndexTypeUInt32;
    }
}

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

void GraphicsBackendMetal::GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
}

void GraphicsBackendMetal::GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
}

void GraphicsBackendMetal::DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
}

void GraphicsBackendMetal::DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
}

void GraphicsBackendMetal::BindTexture(TextureType type, GraphicsBackendTexture texture)
{
}

void GraphicsBackendMetal::BindSampler(TextureUnit unit, GraphicsBackendSampler sampler)
{
}

void GraphicsBackendMetal::GenerateMipmaps(TextureType type)
{
}

void GraphicsBackendMetal::SetTextureParameterInt(TextureType type, TextureParameter parameter, int value)
{
}

void GraphicsBackendMetal::SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value)
{
}

void GraphicsBackendMetal::SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float *valueArray)
{
}

void GraphicsBackendMetal::GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int *outValues)
{
}

void GraphicsBackendMetal::TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                           const void *pixelsData)
{
}

void GraphicsBackendMetal::TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                           const void *pixelsData)
{
}

void GraphicsBackendMetal::TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void *pixelsData)
{
}

void GraphicsBackendMetal::TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void *pixelsData)
{
}

void GraphicsBackendMetal::GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels)
{
}

void GraphicsBackendMetal::GetCompressedTextureImage(TextureTarget target, int level, void *outPixels)
{
}

void GraphicsBackendMetal::SetActiveTextureUnit(TextureUnit unit)
{
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
        if (argument->bufferDataType() == MTL::DataType::DataTypeStruct)
        {
            auto bufferName = argument->name()->utf8String();
            auto bufferIndex = argument->index();

            auto it = buffers.find(bufferName);
            if (it != buffers.end())
            {
                auto bindings = it->second->GetBinding();
                SetBindingIndex(bindings, bufferIndex, isVertexShader);
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
                SetBindingIndex(bindings, bufferIndex, isVertexShader);
                buffer->SetBindings(bindings);

                buffers[bufferName] = buffer;
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
    m_CurrentCommandEncoder->drawPrimitives(ToMetalPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendMetal::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawPrimitives(ToMetalPrimitiveType(primitiveType), firstIndex, indicesCount, indicesCount);
}

void GraphicsBackendMetal::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), ToMetalIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0);
}

void GraphicsBackendMetal::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    m_CurrentCommandEncoder->setVertexBuffer(reinterpret_cast<MTL::Buffer*>(geometry.VertexBuffer.Buffer), 0, 0);
    m_CurrentCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(elementsCount), ToMetalIndicesDataType(dataType), reinterpret_cast<MTL::Buffer*>(geometry.IndexBuffer.Buffer), 0, instanceCount);
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
