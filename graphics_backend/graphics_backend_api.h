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

class GraphicsBackendTexture;
class GraphicsBackendSampler;
class GraphicsBackendBuffer;
class GraphicsBackendFramebuffer;
class GraphicsBackendProgram;
class GraphicsBackendShaderObject;
class GraphicsBackendUniformLocation;
class GraphicsBackendVAO;

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

class GraphicsBackend
{
public:
    static void Init();

    static void GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    static void GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr);
    static void DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr);
    static void DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr);

    static void BindTexture(TextureType type, GraphicsBackendTexture texture);
    static void BindSampler(TextureUnit unit, GraphicsBackendSampler sampler);

    static void GenerateMipmaps(TextureType type);

    static void SetTextureParameterInt(TextureType type, TextureParameter parameter, int value);
    static void SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value);
    static void SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float* valueArray);

    static void GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int* outValues);

    static void TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData);
    static void TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData);
    static void TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void* pixelsData);
    static void TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void* pixelsData);

    static void GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels);
    static void GetCompressedTextureImage(TextureTarget target, int level, void* outPixels);

    static void SetActiveTextureUnit(TextureUnit unit);
    static void GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr);
    static void DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr);
    static void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer);
    static void SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level);
    static void SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer);

    static void GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    static void DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr);
    static void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer);
    static void BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size);

    static void SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint);
    static void SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data);

    static void GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    static void DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr);
    static void BindVertexArrayObject(GraphicsBackendVAO vao);

    static void EnableVertexAttributeArray(int index);
    static void DisableVertexAttributeArray(int index);
    static void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer);
    static void SetVertexAttributeDivisor(int index, int divisor);

    static void SetCapability(GraphicsBackendCapability capability, bool enabled);

    static void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor);
    static void SetCullFace(CullFace cullFace);
    static void SetCullFaceOrientation(CullFaceOrientation orientation);
    static void SetDepthFunction(DepthFunction function);
    static void SetDepthWrite(bool enabled);
    static void SetDepthRange(double near, double far);
    static void SetViewport(int x, int y, int width, int height);

    static GraphicsBackendShaderObject CreateShader(ShaderType shaderType);
    static void DeleteShader(GraphicsBackendShaderObject shader);
    static void SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths);
    static void CompileShader(GraphicsBackendShaderObject shader);
    static void GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int* value);
    static void GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog);
    static bool IsShader(GraphicsBackendShaderObject shader);
    static void AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader);
    static void DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader);

    static GraphicsBackendProgram CreateProgram();
    static void DeleteProgram(GraphicsBackendProgram program);
    static void LinkProgram(GraphicsBackendProgram program);
    static void UseProgram(GraphicsBackendProgram program);
    static void GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int* value);
    static void GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog);
    static bool TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index);
    static void SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding);
    static void GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name);
    static void GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values);
    static GraphicsBackendUniformLocation GetUniformLocation(GraphicsBackendProgram program, const char *uniformName);
    static void SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose = false);
    static void GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values);
    static void GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name);

    static void Clear(ClearMask mask);
    static void SetClearColor(float r, float g, float b, float a);
    static void SetClearDepth(double depth);

    static void DrawArrays(PrimitiveType primitiveType, int firstIndex, int indicesCount);
    static void DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount);
    static void DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices);
    static void DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount);

    static GRAPHICS_BACKEND_TYPE_ENUM GetError();
    static const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error);
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_API_H
