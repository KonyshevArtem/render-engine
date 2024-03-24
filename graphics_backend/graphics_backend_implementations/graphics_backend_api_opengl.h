#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H

#include "graphics_backend_api_base.h"

class GraphicsBackendOpenGL : public GraphicsBackendBase
{
public:
    void Init() override;
    int GetMajorVersion() override;
    int GetMinorVersion() override;
    const std::string &GetShadingLanguageDirective() override;
    GraphicsBackendName GetName() override;

    void GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr) override;
    void GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr) override;
    void DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr) override;
    void DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr) override;

    void BindTexture(TextureType type, GraphicsBackendTexture texture) override;
    void BindSampler(TextureUnit unit, GraphicsBackendSampler sampler) override;

    void GenerateMipmaps(TextureType type) override;

    void SetTextureParameterInt(TextureType type, TextureParameter parameter, int value) override;
    void SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value) override;
    void SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float* valueArray) override;

    void GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int* outValues) override;

    void TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData) override;
    void TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType, const void* pixelsData) override;
    void TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void* pixelsData) override;
    void TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void* pixelsData) override;

    void GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels) override;
    void GetCompressedTextureImage(TextureTarget target, int level, void* outPixels) override;

    void SetActiveTextureUnit(TextureUnit unit) override;
    void GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr) override;
    void DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr) override;
    void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer) override;
    void SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level) override;
    void SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer) override;

    void GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr) override;
    void DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr) override;
    void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer) override;
    void BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size) override;

    void SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint) override;
    void SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data) override;
    void CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size) override;

    void GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr) override;
    void DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr) override;
    void BindVertexArrayObject(GraphicsBackendVAO vao) override;

    void EnableVertexAttributeArray(int index) override;
    void DisableVertexAttributeArray(int index) override;
    void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer) override;
    void SetVertexAttributeDivisor(int index, int divisor) override;

    void SetCapability(GraphicsBackendCapability capability, bool enabled) override;

    void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor) override;
    void SetCullFace(CullFace cullFace) override;
    void SetCullFaceOrientation(CullFaceOrientation orientation) override;
    void SetDepthFunction(DepthFunction function) override;
    void SetDepthWrite(bool enabled) override;
    void SetDepthRange(double near, double far) override;
    void SetViewport(int x, int y, int width, int height) override;

    GraphicsBackendShaderObject CreateShader(ShaderType shaderType) override;
    void DeleteShader(GraphicsBackendShaderObject shader) override;
    void SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths) override;
    void CompileShader(GraphicsBackendShaderObject shader) override;
    void GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int* value) override;
    void GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog) override;
    bool IsShader(GraphicsBackendShaderObject shader) override;
    void AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader) override;
    void DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader) override;

    GraphicsBackendProgram CreateProgram() override;
    void DeleteProgram(GraphicsBackendProgram program) override;
    void LinkProgram(GraphicsBackendProgram program) override;
    void UseProgram(GraphicsBackendProgram program) override;
    void GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int* value) override;
    void GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog) override;
    bool TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index) override;
    void SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding) override;
    void GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name) override;
    void GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values) override;
    GraphicsBackendUniformLocation GetUniformLocation(GraphicsBackendProgram program, const char *uniformName) override;
    void SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose = false) override;
    void GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values) override;
    void GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name) override;

    void Clear(ClearMask mask) override;
    void SetClearColor(float r, float g, float b, float a) override;
    void SetClearDepth(double depth) override;

    void DrawArrays(PrimitiveType primitiveType, int firstIndex, int indicesCount) override;
    void DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) override;
    void DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices) override;
    void DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount) override;

    void GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues) override;
    void GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues) override;
    void GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName) override;

    bool SupportShaderStorageBuffer() override;
    void SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding) override;

    void BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter) override;

    void PushDebugGroup(const std::string& name, int id) override;
    void PopDebugGroup() override;

    GRAPHICS_BACKEND_TYPE_ENUM GetError() override;
    const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error) override;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
