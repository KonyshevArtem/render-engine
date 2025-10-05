#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H

#ifdef RENDER_BACKEND_METAL

#include "graphics_backend_api_base.h"

#include <shared_mutex>

namespace MTL
{
    class Device;
    class CommandBuffer;
    class RenderPassDescriptor;
    class RenderCommandEncoder;
    class BlitCommandEncoder;
    class Texture;
    class CommandQueue;
}

namespace MTK
{
    class View;
}

class GraphicsBackendMetal : public GraphicsBackendBase
{
public:
    void Init(void *data) override;
    GraphicsBackendName GetName() override;
    void InitNewFrame() override;
    void FillImGuiInitData(void* data) override;
    void FillImGuiFrameData(void* data) override;

    GraphicsBackendTexture CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name) override;
    GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, ComparisonFunction comparisonFunction, const std::string& name) override;

    void GenerateMipmaps(const GraphicsBackendTexture &texture) override;
    void UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData) override;

    void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) override;
    TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear) override;

    GraphicsBackendBuffer CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data) override;

    void SetBufferData(const GraphicsBackendBuffer &buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(const GraphicsBackendBuffer &source, const GraphicsBackendBuffer &destination, int sourceOffset, int destinationOffset, int size) override;
    uint64_t GetMaxConstantBufferSize() override;
    int GetConstantBufferOffsetAlignment() override;

    GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name) override;

    void SetViewport(int x, int y, int width, int height, float near, float far) override;
    void SetScissorRect(int x, int y, int width, int height) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) override;
    GraphicsBackendShaderObject CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name) override;
    GraphicsBackendProgram CreateProgram(const GraphicsBackendProgramDescriptor& descriptor) override;
    void UseProgram(GraphicsBackendProgram program) override;
    bool RequireStrictPSODescriptor() override;

    void SetClearColor(float r, float g, float b, float a) override;
    void SetClearDepth(double depth) override;

    void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) override;
    void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) override;
    void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) override;
    void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) override;

    void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) override;

    void PushDebugGroup(const std::string& name, GPUQueue queue) override;
    void PopDebugGroup(GPUQueue queue) override;
    GraphicsBackendProfilerMarker PushProfilerMarker() override;
    void PopProfilerMarker(GraphicsBackendProfilerMarker& marker) override;
    bool ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResults& outResults) override;

    void BeginRenderPass(const std::string& name) override;
    void EndRenderPass() override;
    void BeginCopyPass(const std::string& name) override;
    void EndCopyPass() override;

    GraphicsBackendFence CreateFence(FenceType fenceType, const std::string& name) override;
    void DeleteFence(const GraphicsBackendFence& fence) override;
    void SignalFence(const GraphicsBackendFence& fence) override;
    void WaitForFence(const GraphicsBackendFence& fence) override;

    void Flush() override;
    void Present() override;

    void TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor, ResourceState state, GPUQueue queue) override;
    void TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue) override;
    void TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue) override;

protected:
    void DeleteTexture_Internal(const GraphicsBackendTexture &texture) override;
    void DeleteSampler_Internal(const GraphicsBackendSampler &sampler) override;
    void DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer) override;
    void DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry) override;
    void DeleteShader_Internal(GraphicsBackendShaderObject shader) override;
    void DeleteProgram_Internal(GraphicsBackendProgram program) override;

    void BindTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index) override;
    void BindSampler_Internal(const GraphicsBackendSampler& sampler, uint32_t index) override;
    void BindBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size) override;
    void BindStructuredBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int count) override;
    void BindConstantBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size) override;

private:
    MTL::Device* m_Device = nullptr;
    MTK::View* m_View = nullptr;
    MTL::CommandQueue* m_RenderCommandQueue = nullptr;
    MTL::CommandQueue* m_CopyCommandQueue = nullptr;
    MTL::CommandBuffer* m_RenderCommandBuffer = nullptr;
    MTL::CommandBuffer* m_CopyCommandBuffer = nullptr;
    MTL::RenderPassDescriptor* m_BackbufferDescriptor = nullptr;
    MTL::RenderPassDescriptor* m_RenderPassDescriptor = nullptr;
    MTL::RenderCommandEncoder* m_RenderCommandEncoder = nullptr;

    std::shared_mutex m_UploadCommandBuffersMutex;
    std::unordered_map<std::thread::id, MTL::CommandBuffer*> m_UploadCommandBuffers;

    std::shared_mutex m_BlitCommandEncodersMutex;
    std::unordered_map<std::thread::id, MTL::BlitCommandEncoder*> m_BlitCommandEncoders;

    bool m_SupportTimestampCounters = false;
    bool m_ProfilerMarkerActive = false;

    MTL::Texture* GetTextureFromDescriptor(const GraphicsBackendRenderTargetDescriptor& descriptor);
    void SetCommandBuffers(MTL::CommandBuffer* renderCommandBuffer, MTL::CommandBuffer* copyCommandBuffer);

    MTL::CommandBuffer* GetOrCreateUploadCommandBuffer();
    void SetUploadCommandBuffer(MTL::CommandBuffer* buffer);

    MTL::BlitCommandEncoder* GetBlitCommandEncoder();
    void SetBlitCommandEncoder(MTL::BlitCommandEncoder* encoder);
};


#endif // RENDER_BACKEND_METAL
#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_METAL_H