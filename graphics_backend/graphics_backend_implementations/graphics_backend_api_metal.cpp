#if RENDER_ENGINE_APPLE

#include "graphics_backend_api_metal.h"
#include "graphics_backend_debug.h"
#include "enums/texture_unit.h"
#include "enums/uniform_data_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_framebuffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_uniform_location.h"
#include "types/graphics_backend_vao.h"


void GraphicsBackendMetal::Init()
{
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
    return "";
}

GraphicsBackendName GraphicsBackendMetal::GetName()
{
    return GraphicsBackendName::METAL;
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

void GraphicsBackendMetal::GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
}

void GraphicsBackendMetal::DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
}

void GraphicsBackendMetal::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
}

void GraphicsBackendMetal::BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size)
{
}

void GraphicsBackendMetal::SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint)
{
}

void GraphicsBackendMetal::SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data)
{
}

void GraphicsBackendMetal::CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size)
{
}

void GraphicsBackendMetal::GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
}

void GraphicsBackendMetal::DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
}

void GraphicsBackendMetal::BindVertexArrayObject(GraphicsBackendVAO vao)
{
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

GraphicsBackendShaderObject GraphicsBackendMetal::CreateShader(ShaderType shaderType)
{
    GraphicsBackendShaderObject shader{};
    shader.ShaderObject = 0;
    return shader;
}

void GraphicsBackendMetal::DeleteShader(GraphicsBackendShaderObject shader)
{
}

void GraphicsBackendMetal::SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths)
{
}

void GraphicsBackendMetal::CompileShader(GraphicsBackendShaderObject shader)
{
}

void GraphicsBackendMetal::GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int *value)
{
}

void GraphicsBackendMetal::GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog)
{
}

bool GraphicsBackendMetal::IsShader(GraphicsBackendShaderObject shader)
{
    return false;
}

void GraphicsBackendMetal::AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
}

void GraphicsBackendMetal::DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
}

GraphicsBackendProgram GraphicsBackendMetal::CreateProgram()
{
    GraphicsBackendProgram program{};
    program.Program = 0;
    return program;
}

void GraphicsBackendMetal::DeleteProgram(GraphicsBackendProgram program)
{
}

void GraphicsBackendMetal::LinkProgram(GraphicsBackendProgram program)
{
}

void GraphicsBackendMetal::GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int *value)
{
}

void GraphicsBackendMetal::GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog)
{
}

bool GraphicsBackendMetal::TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index)
{
    return false;
}

void GraphicsBackendMetal::SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding)
{
}

void GraphicsBackendMetal::GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name)
{
}

void GraphicsBackendMetal::GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values)
{
}

void GraphicsBackendMetal::GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name)
{
}

GraphicsBackendUniformLocation GraphicsBackendMetal::GetUniformLocation(GraphicsBackendProgram program, const char *uniformName)
{
    GraphicsBackendUniformLocation location{};
    location.UniformLocation = 0;
    return location;
}

void GraphicsBackendMetal::UseProgram(GraphicsBackendProgram program)
{
}

void GraphicsBackendMetal::SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose)
{
}

void GraphicsBackendMetal::GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values)
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

void GraphicsBackendMetal::DrawArrays(PrimitiveType primitiveType, int firstIndex, int count)
{
}

void GraphicsBackendMetal::DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
}

void GraphicsBackendMetal::DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices)
{
}

void GraphicsBackendMetal::DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount)
{
}

void GraphicsBackendMetal::GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues)
{
}

void GraphicsBackendMetal::GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues)
{
}

void GraphicsBackendMetal::GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName)
{
}

bool GraphicsBackendMetal::SupportShaderStorageBuffer()
{
    return false;
}

void GraphicsBackendMetal::SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding)
{
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

GRAPHICS_BACKEND_TYPE_ENUM GraphicsBackendMetal::GetError()
{
    return 0;
}

const char *GraphicsBackendMetal::GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error)
{
    return nullptr;
}

#endif
