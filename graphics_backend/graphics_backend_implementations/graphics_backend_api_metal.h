#if RENDER_ENGINE_APPLE

#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H

#include "graphics_backend_api_base.h"

namespace MTL
{
    class Device;
    class CommandBuffer;
    class RenderPassDescriptor;
    class RenderCommandEncoder;
}

class GraphicsBackendMetal : public GraphicsBackendBase
{
public:
    void Init(void *device) override;
    int GetMajorVersion() override;
    int GetMinorVersion() override;
    const std::string &GetShadingLanguageDirective() override;
    GraphicsBackendName GetName() override;
    void PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor) override;

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

    GraphicsBackendBuffer CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint) override;
    void DeleteBuffer(const GraphicsBackendBuffer &buffer) override;
    void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer) override;
    void BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;

    void SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size) override;

    GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes) override;
    void DeleteGeometry(const GraphicsBackendGeometry &geometry) override;

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

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source) override;
    GraphicsBackendProgram CreateProgram(GraphicsBackendShaderObject *shaders, int shadersCount) override;
    void DeleteProgram(GraphicsBackendProgram program) override;
    void UseProgram(GraphicsBackendProgram program) override;
    void SetUniform(int location, UniformDataType dataType, int count, const void *data, bool transpose = false) override;
    void IntrospectProgram(GraphicsBackendProgram program, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers) override;

    void Clear(ClearMask mask) override;
    void SetClearColor(float r, float g, float b, float a) override;
    void SetClearDepth(double depth) override;

    void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) override;
    void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) override;
    void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) override;
    void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) override;

    bool SupportShaderStorageBuffer() override;

    void BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter) override;

    void PushDebugGroup(const std::string& name, int id) override;
    void PopDebugGroup() override;

    void BeginRenderPass() override;
    void EndRenderPass() override;

    GRAPHICS_BACKEND_TYPE_ENUM GetError() override;
    const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error) override;

private:
    MTL::Device *m_Device = nullptr;
    MTL::CommandBuffer *m_CommandBuffer = nullptr;
    MTL::RenderPassDescriptor *m_BackbufferDescriptor = nullptr;
    MTL::RenderCommandEncoder *m_CurrentCommandEncoder = nullptr;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H

#endif