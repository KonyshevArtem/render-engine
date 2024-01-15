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

GraphicsBackendError GraphicsBackend::GetError()
{
    return glGetError();
}

const char *GraphicsBackend::GetErrorString(GraphicsBackendError error)
{
    return reinterpret_cast<const char *>(gluGetString(error));
}