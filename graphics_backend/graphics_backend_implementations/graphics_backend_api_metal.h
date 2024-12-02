#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H

#ifdef RENDER_BACKEND_METAL

#include "graphics_backend_api_base.h"

namespace MTL
{
    class Device;
    class CommandBuffer;
    class RenderPassDescriptor;
    class RenderCommandEncoder;
    class BlitCommandEncoder;
    class RenderPipelineReflection;
    class Texture;

    enum PixelFormat : std::uintptr_t;
}

class GraphicsBackendMetal : public GraphicsBackendBase
{
public:
    void Init(void *data) override;
    GraphicsBackendName GetName() override;
    void InitNewFrame(void *data) override;

    GraphicsBackendTexture CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name) override;
    GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, const std::string& name) override;
    void DeleteTexture(const GraphicsBackendTexture &texture) override;
    void DeleteSampler(const GraphicsBackendSampler &sampler) override;

    void BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture) override;
    void BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler) override;

    void GenerateMipmaps(const GraphicsBackendTexture &texture) override;
    void UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData) override;

    void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) override;
    TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear) override;

    GraphicsBackendBuffer CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data) override;
    void DeleteBuffer(const GraphicsBackendBuffer &buffer) override;
    void BindBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;
    void BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;

    void SetBufferData(const GraphicsBackendBuffer &buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(const GraphicsBackendBuffer &source, const GraphicsBackendBuffer &destination, int sourceOffset, int destinationOffset, int size) override;
    uint64_t GetMaxConstantBufferSize() override;
    int GetConstantBufferOffsetAlignment() override;

    GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name) override;
    void DeleteGeometry(const GraphicsBackendGeometry &geometry) override;

    void SetCullFace(CullFace cullFace) override;
    void SetCullFaceOrientation(CullFaceOrientation orientation) override;
    void SetViewport(int x, int y, int width, int height, float near, float far) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) override;
    GraphicsBackendProgram CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name) override;
    void DeleteShader(GraphicsBackendShaderObject shader) override;
    void DeleteProgram(GraphicsBackendProgram program) override;
    void UseProgram(GraphicsBackendProgram program) override;
    bool RequireStrictPSODescriptor() override;

    void SetClearColor(float r, float g, float b, float a) override;
    void SetClearDepth(double depth) override;

    void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) override;
    void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) override;
    void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) override;
    void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) override;

    void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) override;

    void PushDebugGroup(const std::string& name) override;
    void PopDebugGroup() override;

    void BeginRenderPass(const std::string& name) override;
    void EndRenderPass() override;
    void BeginCopyPass(const std::string& name) override;
    void EndCopyPass() override;

    GraphicsBackendDepthStencilState CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction, const std::string& name) override;
    void DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state) override;
    void SetDepthStencilState(const GraphicsBackendDepthStencilState& state) override;

    GraphicsBackendFence CreateFence(FenceType fenceType, const std::string& name) override;
    void SignalFence(const GraphicsBackendFence& fence) override;
    void WaitForFence(const GraphicsBackendFence& fence) override;

private:
    MTL::Device* m_Device = nullptr;
    MTL::CommandBuffer* m_RenderCommandBuffer = nullptr;
    MTL::CommandBuffer* m_CopyCommandBuffer = nullptr;
    MTL::RenderPassDescriptor* m_BackbufferDescriptor = nullptr;
    MTL::RenderPassDescriptor* m_RenderPassDescriptor = nullptr;
    MTL::RenderCommandEncoder* m_RenderCommandEncoder = nullptr;
    MTL::BlitCommandEncoder* m_BlitCommandEncoder = nullptr;

    MTL::Texture* GetTextureFromDescriptor(const GraphicsBackendRenderTargetDescriptor& descriptor);
    void SetCommandBuffers(MTL::CommandBuffer* renderCommandBuffer, MTL::CommandBuffer* copyCommandBuffer);
};


#endif // RENDER_BACKEND_METAL
#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H