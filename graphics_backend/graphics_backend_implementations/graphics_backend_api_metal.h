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

    enum PixelFormat : std::uintptr_t;
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

    GraphicsBackendTexture CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels, bool isRenderTarget) override;
    GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod) override;
    void DeleteTexture(const GraphicsBackendTexture &texture) override;
    void DeleteSampler(const GraphicsBackendSampler &sampler) override;

    void BindTexture(const GraphicsBackendResourceBindings &bindings, int uniformLocation, const GraphicsBackendTexture &texture) override;
    void BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler) override;

    void GenerateMipmaps(const GraphicsBackendTexture &texture) override;

    void UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData) override;
    void DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels) override;
    TextureInternalFormat GetTextureFormat(const GraphicsBackendTexture &texture) override;
    int GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice) override;

    void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer) override;
    void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) override;

    GraphicsBackendBuffer CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint) override;
    void DeleteBuffer(const GraphicsBackendBuffer &buffer) override;
    void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer) override;
    void BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;

    void SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size) override;
    uint64_t GetMaxConstantBufferSize() override;
    int GetConstantBufferOffsetAlignment() override;

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
    void SetViewport(int x, int y, int width, int height) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source) override;
    GraphicsBackendProgram CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, TextureInternalFormat colorFormat, TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes) override;
    void DeleteShader(GraphicsBackendShaderObject shader) override;
    void DeleteProgram(GraphicsBackendProgram program) override;
    void UseProgram(GraphicsBackendProgram program) override;
    void SetUniform(int location, UniformDataType dataType, int count, const void *data, bool transpose = false) override;
    void IntrospectProgram(GraphicsBackendProgram program, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers) override;
    bool RequireStrictPSODescriptor() override;

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

    GraphicsBackendDepthStencilState CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction) override;
    void DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state) override;
    void SetDepthStencilState(const GraphicsBackendDepthStencilState& state) override;

    GRAPHICS_BACKEND_TYPE_ENUM GetError() override;
    const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error) override;

private:
    MTL::Device *m_Device = nullptr;
    MTL::CommandBuffer *m_CommandBuffer = nullptr;
    MTL::RenderPassDescriptor *m_BackbufferDescriptor = nullptr;
    MTL::RenderPassDescriptor *m_RenderPassDescriptor = nullptr;
    MTL::RenderCommandEncoder *m_CurrentCommandEncoder = nullptr;

    MTL::PixelFormat GetPSOPixelFormat(TextureInternalFormat textureFormat, bool isColor, int index);
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H

#endif