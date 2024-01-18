#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"

#include <type_traits>

template<typename T>
constexpr auto Cast(T value) -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

void GraphicsBackend::GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenTextures(texturesCount, texturesPtr))
}

void GraphicsBackend::GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenSamplers(samplersCount, samplersPtr))
}

void GraphicsBackend::DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteTextures(texturesCount, texturesPtr))
}

void GraphicsBackend::DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteSamplers(samplersCount, samplersPtr))
}

void GraphicsBackend::BindTexture(TextureType type, GraphicsBackendTexture texture)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(Cast(type), texture))
}

void GraphicsBackend::BindSampler(TextureUnit unit, GraphicsBackendSampler sampler)
{
    auto unitIndex = TextureUnitToIndex(unit);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindSampler(unitIndex, sampler))
}

void GraphicsBackend::GenerateMipmaps(TextureType type)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenerateMipmap(Cast(type)))
}

void GraphicsBackend::SetTextureParameterInt(TextureType type, TextureParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(Cast(type), Cast(parameter), value))
}

void GraphicsBackend::SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler, Cast(parameter), value))
}

void GraphicsBackend::SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float *valueArray)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameterfv(sampler, Cast(parameter), valueArray))
}

void GraphicsBackend::GetTextureLevelParameterInt(TextureTarget target, GraphicsBackendTextureLevel level, TextureLevelParameter parameter, int *outValues)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexLevelParameteriv(Cast(target), level, Cast(parameter), outValues))
}

void GraphicsBackend::TextureImage2D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackend::TextureImage3D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackend::TextureCompressedImage2D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, imageSize, pixelsData))
}

void GraphicsBackend::TextureCompressedImage3D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, imageSize, pixelsData))
}

void GraphicsBackend::GetTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexImage(Cast(target), level, Cast(pixelFormat), Cast(dataType), outPixels))
}

void GraphicsBackend::GetCompressedTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetCompressedTexImage(Cast(target), level, outPixels))
}

void GraphicsBackend::SetActiveTextureUnit(TextureUnit unit)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(Cast(unit)))
}

void GraphicsBackend::SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTexture(Cast(target), Cast(attachment), texture, level))
}

void GraphicsBackend::SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level, GraphicsBackendTextureLayer layer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTextureLayer(Cast(target), Cast(attachment), texture, level, layer))
}

void GraphicsBackend::GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenBuffers(buffersCount, buffersPtr))
}

void GraphicsBackend::DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteBuffers(buffersCount, buffersPtr))
}

void GraphicsBackend::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(Cast(target), buffer))
}

void GraphicsBackend::SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferData(Cast(target), size, data, Cast(usageHint)))
}

void GraphicsBackend::SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferSubData(Cast(target), offset, size, data))
}

void GraphicsBackend::GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenVertexArrays(vaoCount, vaoPtr))
}

void GraphicsBackend::DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteVertexArrays(vaoCount, vaoPtr))
}

void GraphicsBackend::BindVertexArrayObject(GraphicsBackendVAO vao)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(vao))
}

void GraphicsBackend::EnableVertexAttributeArray(int index)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glEnableVertexAttribArray(index))
}

void GraphicsBackend::SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribPointer(index, size, Cast(dataType), normalized ? GL_TRUE : GL_FALSE, stride, pointer))
}

void GraphicsBackend::SetCapability(GraphicsBackendCapability capability, bool enabled)
{
    if (enabled)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glEnable(Cast(capability)))
    }
    else
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glDisable(Cast(capability)))
    }
}

void GraphicsBackend::SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBlendFunc(Cast(sourceFactor), Cast(destinationFactor)))
}

void GraphicsBackend::SetCullFace(CullFace cullFace)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCullFace(Cast(cullFace)))
}

void GraphicsBackend::SetDepthFunction(DepthFunction function)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthFunc(Cast(function)))
}

void GraphicsBackend::SetDepthWrite(bool enabled)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthMask(enabled ? GL_TRUE : GL_FALSE))
}

GraphicsBackendShaderObject GraphicsBackend::CreateShader(ShaderType shaderType)
{
    auto shader = CHECK_GRAPHICS_BACKEND_FUNC(glCreateShader(Cast(shaderType)))
    return shader;
}

void GraphicsBackend::DeleteShader(GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteShader(shader))
}

void GraphicsBackend::SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glShaderSource(shader, sourcesCount, sources, sourceLengths))
}

void GraphicsBackend::CompileShader(GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompileShader(shader))
}

void GraphicsBackend::GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int *value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderiv(shader, Cast(parameter), value))
}

void GraphicsBackend::GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderInfoLog(shader, maxLength, length, infoLog))
}

bool GraphicsBackend::IsShader(GraphicsBackendShaderObject shader)
{
    auto isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader))
    return isShader == GL_TRUE;
}

void GraphicsBackend::AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glAttachShader(program, shader))
}

void GraphicsBackend::DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDetachShader(program, shader))
}

GraphicsBackendProgram GraphicsBackend::CreateProgram()
{
    auto program = CHECK_GRAPHICS_BACKEND_FUNC(glCreateProgram())
    return program;
}

void GraphicsBackend::DeleteProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteProgram(program))
}

void GraphicsBackend::LinkProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glLinkProgram(program))
}

void GraphicsBackend::GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int *value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program, Cast(parameter), value))
}

void GraphicsBackend::GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInfoLog(program, maxLength, length, infoLog))
}

GraphicsBackendError GraphicsBackend::GetError()
{
    return glGetError();
}

const char *GraphicsBackend::GetErrorString(GraphicsBackendError error)
{
    return reinterpret_cast<const char *>(gluGetString(error));
}