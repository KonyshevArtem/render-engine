#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_API_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_API_H

#include "graphics_backend.h"
#include "enums/texture_type.h"
#include "enums/texture_parameter.h"
#include "enums/texture_target.h"
#include "enums/texture_level_parameter.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_pixel_format.h"
#include "enums/texture_data_type.h"
#include "enums/texture_unit.h"
#include "enums/framebuffer_attachment.h"
#include "enums/framebuffer_target.h"
#include "enums/sampler_parameter.h"
#include "enums/primitive_type.h"
#include "enums/buffer_bind_target.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/buffer_usage_hint.h"

#ifdef OPENGL_STUDY_EDITOR
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction)                   \
    backendFunction;                                                   \
    {                                                                  \
        auto error = GraphicsBackend::GetError();                      \
        if (error != 0)                                                \
            GraphicsBackendDebug::LogError(error, __FILE__, __LINE__); \
    }
#else
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction) backendFunction;
#endif

namespace GraphicsBackend
{
    void GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    void GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr);
    void DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    void DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr);

    void BindTexture(TextureType type, GraphicsBackendTexture texture);
    void BindSampler(TextureUnit unit, GraphicsBackendSampler sampler);

    void GenerateMipmaps(TextureType type);

    void SetTextureParameterInt(TextureType type, TextureParameter parameter, int value);
    void SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value);
    void SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float* valueArray);

    void GetTextureLevelParameterInt(TextureTarget target, GraphicsBackendTextureLevel level, TextureLevelParameter parameter, int* outValues);

    void TextureImage2D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData);
    void TextureImage3D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData);
    void TextureCompressedImage2D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void* pixelsData);
    void TextureCompressedImage3D(TextureTarget target, GraphicsBackendTextureLevel level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void* pixelsData);

    void GetTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels);
    void GetCompressedTextureImage(TextureTarget target, GraphicsBackendTextureLevel level, void* outPixels);

    void SetActiveTextureUnit(TextureUnit unit);
    void SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level);
    void SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level, GraphicsBackendTextureLayer layer);

    void GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    void DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer);

    void SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint);
    void SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data);

    void GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    void DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    void BindVertexArrayObject(GraphicsBackendVAO vao);

    void EnableVertexAttributeArray(int index);
    void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer);

    GraphicsBackendError GetError();
    const char *GetErrorString(GraphicsBackendError error);
}

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_API_H
