#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H

#include "graphics_backend.h"
#include "enums/graphics_backend_name.h"

#include <string>

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
FORWARD_DECLARE_ENUM(BlitFramebufferMask)
FORWARD_DECLARE_ENUM(BlitFramebufferFilter)
#undef FORWARD_DECLARE_ENUM

class GraphicsBackendTexture;
class GraphicsBackendSampler;
class GraphicsBackendBuffer;
class GraphicsBackendFramebuffer;
class GraphicsBackendProgram;
class GraphicsBackendShaderObject;
class GraphicsBackendUniformLocation;
class GraphicsBackendVAO;

class GraphicsBackendBase
{
public:
    static GraphicsBackendBase *Create(const std::string &backend);

    virtual void Init() = 0;
    virtual int GetMajorVersion() = 0;
    virtual int GetMinorVersion() = 0;
    virtual const std::string &GetShadingLanguageDirective() = 0;
    virtual GraphicsBackendName GetName() = 0;

    virtual void GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr) = 0;
    virtual void GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr) = 0;
    virtual void DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr) = 0;
    virtual void DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr) = 0;

    virtual void BindTexture(TextureType type, GraphicsBackendTexture texture) = 0;
    virtual void BindSampler(TextureUnit unit, GraphicsBackendSampler sampler) = 0;

    virtual void GenerateMipmaps(TextureType type) = 0;

    virtual void SetTextureParameterInt(TextureType type, TextureParameter parameter, int value) = 0;
    virtual void SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value) = 0;
    virtual void SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float* valueArray) = 0;

    virtual void GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int* outValues) = 0;

    virtual void TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData) = 0;
    virtual void TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData) = 0;
    virtual void TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void* pixelsData) = 0;
    virtual void TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void* pixelsData) = 0;

    virtual void GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels) = 0;
    virtual void GetCompressedTextureImage(TextureTarget target, int level, void* outPixels) = 0;

    virtual void SetActiveTextureUnit(TextureUnit unit) = 0;
    virtual void GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr) = 0;
    virtual void DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr) = 0;
    virtual void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer) = 0;
    virtual void SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level) = 0;
    virtual void SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer) = 0;

    virtual void GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr) = 0;
    virtual void DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr) = 0;
    virtual void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer) = 0;
    virtual void BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size) = 0;

    virtual void SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint) = 0;
    virtual void SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data) = 0;
    virtual void CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size) = 0;

    virtual void GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr) = 0;
    virtual void DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr) = 0;
    virtual void BindVertexArrayObject(GraphicsBackendVAO vao) = 0;

    virtual void EnableVertexAttributeArray(int index) = 0;
    virtual void DisableVertexAttributeArray(int index) = 0;
    virtual void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer) = 0;
    virtual void SetVertexAttributeDivisor(int index, int divisor) = 0;

    virtual void SetCapability(GraphicsBackendCapability capability, bool enabled) = 0;

    virtual void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor) = 0;
    virtual void SetCullFace(CullFace cullFace) = 0;
    virtual void SetCullFaceOrientation(CullFaceOrientation orientation) = 0;
    virtual void SetDepthFunction(DepthFunction function) = 0;
    virtual void SetDepthWrite(bool enabled) = 0;
    virtual void SetDepthRange(double near, double far) = 0;
    virtual void SetViewport(int x, int y, int width, int height) = 0;

    virtual GraphicsBackendShaderObject CreateShader(ShaderType shaderType) = 0;
    virtual void DeleteShader(GraphicsBackendShaderObject shader) = 0;
    virtual void SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths) = 0;
    virtual void CompileShader(GraphicsBackendShaderObject shader) = 0;
    virtual void GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int* value) = 0;
    virtual void GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog) = 0;
    virtual bool IsShader(GraphicsBackendShaderObject shader) = 0;
    virtual void AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader) = 0;
    virtual void DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader) = 0;

    virtual GraphicsBackendProgram CreateProgram() = 0;
    virtual void DeleteProgram(GraphicsBackendProgram program) = 0;
    virtual void LinkProgram(GraphicsBackendProgram program) = 0;
    virtual void UseProgram(GraphicsBackendProgram program) = 0;
    virtual void GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int* value) = 0;
    virtual void GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog) = 0;
    virtual bool TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index) = 0;
    virtual void SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding) = 0;
    virtual void GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name) = 0;
    virtual void GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values) = 0;
    virtual GraphicsBackendUniformLocation GetUniformLocation(GraphicsBackendProgram program, const char *uniformName) = 0;
    virtual void SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose = false) = 0;
    virtual void GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values) = 0;
    virtual void GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name) = 0;

    virtual void Clear(ClearMask mask) = 0;
    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void SetClearDepth(double depth) = 0;

    virtual void DrawArrays(PrimitiveType primitiveType, int firstIndex, int indicesCount) = 0;
    virtual void DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) = 0;
    virtual void DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices) = 0;
    virtual void DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount) = 0;

    virtual void GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues) = 0;
    virtual void GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues) = 0;
    virtual void GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName) = 0;

    virtual bool SupportShaderStorageBuffer() = 0;
    virtual void SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding) = 0;

    virtual void BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter) = 0;

    virtual void PushDebugGroup(const std::string& name, int id) = 0;
    virtual void PopDebugGroup() = 0;

    virtual GRAPHICS_BACKEND_TYPE_ENUM GetError() = 0;
    virtual const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error) = 0;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
