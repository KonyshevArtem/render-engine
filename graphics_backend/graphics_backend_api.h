#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_API_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_API_H

#include "graphics_backend.h"

#define FORWARD_DECLARE_ENUM(name) enum class name : GRAPHICS_BACKEND_TYPE_ENUM;
FORWARD_DECLARE_ENUM(TextureType)
FORWARD_DECLARE_ENUM(TextureParameter)
FORWARD_DECLARE_ENUM(TextureTarget)
FORWARD_DECLARE_ENUM(TextureLevelParameter)
FORWARD_DECLARE_ENUM(TextureInternalFormat)
FORWARD_DECLARE_ENUM(TexturePixelFormat)
FORWARD_DECLARE_ENUM(TextureDataType)
FORWARD_DECLARE_ENUM(TextureUnit)
FORWARD_DECLARE_ENUM(FramebufferAttachment)
FORWARD_DECLARE_ENUM(FramebufferTarget)
FORWARD_DECLARE_ENUM(SamplerParameter)
FORWARD_DECLARE_ENUM(PrimitiveType)
FORWARD_DECLARE_ENUM(BufferBindTarget)
FORWARD_DECLARE_ENUM(VertexAttributeDataType)
FORWARD_DECLARE_ENUM(BufferUsageHint)
FORWARD_DECLARE_ENUM(GraphicsBackendCapability)
FORWARD_DECLARE_ENUM(BlendFactor)
FORWARD_DECLARE_ENUM(CullFace)
FORWARD_DECLARE_ENUM(DepthFunction)
FORWARD_DECLARE_ENUM(ShaderType)
FORWARD_DECLARE_ENUM(ShaderParameter)
FORWARD_DECLARE_ENUM(ProgramParameter)
FORWARD_DECLARE_ENUM(UniformDataType)
FORWARD_DECLARE_ENUM(ClearMask)
FORWARD_DECLARE_ENUM(UniformBlockParameter)
FORWARD_DECLARE_ENUM(UniformParameter)
FORWARD_DECLARE_ENUM(CullFaceOrientation)
FORWARD_DECLARE_ENUM(IndicesDataType)
FORWARD_DECLARE_ENUM(ProgramInterface)
FORWARD_DECLARE_ENUM(ProgramInterfaceParameter)
FORWARD_DECLARE_ENUM(ProgramResourceParameter)
#undef FORWARD_DECLARE_ENUM

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

    static void GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues);
    static void GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues);
    static void GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName);

    static bool SupportShaderStorageBuffer();
    static void SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding);

    static GRAPHICS_BACKEND_TYPE_ENUM GetError();
    static const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error);
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_API_H
