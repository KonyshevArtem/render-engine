#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_API_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_API_H

#include "graphics_backend.h"

enum class TextureType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureTarget : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureLevelParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureInternalFormat : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TexturePixelFormat : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureDataType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureUnit : GRAPHICS_BACKEND_TYPE_ENUM;
enum class FramebufferAttachment : GRAPHICS_BACKEND_TYPE_ENUM;
enum class FramebufferTarget : GRAPHICS_BACKEND_TYPE_ENUM;
enum class SamplerParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class PrimitiveType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class BufferBindTarget : GRAPHICS_BACKEND_TYPE_ENUM;
enum class VertexAttributeDataType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class BufferUsageHint : GRAPHICS_BACKEND_TYPE_ENUM;
enum class GraphicsBackendCapability : GRAPHICS_BACKEND_TYPE_ENUM;
enum class BlendFactor : GRAPHICS_BACKEND_TYPE_ENUM;
enum class CullFace : GRAPHICS_BACKEND_TYPE_ENUM;
enum class DepthFunction : GRAPHICS_BACKEND_TYPE_ENUM;
enum class ShaderType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class ShaderParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class ProgramParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class UniformDataType : GRAPHICS_BACKEND_TYPE_ENUM;
enum class ClearMask : GRAPHICS_BACKEND_TYPE_ENUM;
enum class UniformBlockParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class UniformParameter : GRAPHICS_BACKEND_TYPE_ENUM;
enum class CullFaceOrientation : GRAPHICS_BACKEND_TYPE_ENUM;
enum class IndicesDataType : GRAPHICS_BACKEND_TYPE_ENUM;

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
    void GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr);
    void DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr);
    void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer);
    void SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level);
    void SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, GraphicsBackendTextureLevel level, GraphicsBackendTextureLayer layer);

    void GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    void DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer);
    void BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size);

    void SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint);
    void SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data);

    void GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    void DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    void BindVertexArrayObject(GraphicsBackendVAO vao);

    void EnableVertexAttributeArray(int index);
    void DisableVertexAttributeArray(int index);
    void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer);
    void SetVertexAttributeDivisor(int index, int divisor);

    void SetCapability(GraphicsBackendCapability capability, bool enabled);

    void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor);
    void SetCullFace(CullFace cullFace);
    void SetCullFaceOrientation(CullFaceOrientation orientation);
    void SetDepthFunction(DepthFunction function);
    void SetDepthWrite(bool enabled);
    void SetDepthRange(double near, double far);
    void SetViewport(int x, int y, int width, int height);

    GraphicsBackendShaderObject CreateShader(ShaderType shaderType);
    void DeleteShader(GraphicsBackendShaderObject shader);
    void SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths);
    void CompileShader(GraphicsBackendShaderObject shader);
    void GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int* value);
    void GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog);
    bool IsShader(GraphicsBackendShaderObject shader);
    void AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader);
    void DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader);

    GraphicsBackendProgram CreateProgram();
    void DeleteProgram(GraphicsBackendProgram program);
    void LinkProgram(GraphicsBackendProgram program);
    void UseProgram(GraphicsBackendProgram program);
    void GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int* value);
    void GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog);
    bool TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index);
    void SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding);
    void GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name);
    void GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values);
    GraphicsBackendUniformLocation GetUniformLocation(GraphicsBackendProgram program, const char *uniformName);
    void SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose = false);
    void GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values);

    void Clear(ClearMask mask);
    void SetClearColor(float r, float g, float b, float a);
    void SetClearDepth(double depth);

    void DrawArrays(PrimitiveType primitiveType, int firstIndex, int indicesCount);
    void DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount);
    void DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices);
    void DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount);

    GraphicsBackendError GetError();
    const char *GetErrorString(GraphicsBackendError error);
}

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_API_H
