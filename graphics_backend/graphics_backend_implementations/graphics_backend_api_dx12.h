#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_DX12_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_DX12_H

#ifdef RENDER_BACKEND_DX12

#include "graphics_backend_api_base.h"
#include "types/graphics_backend_program_descriptor.h"

namespace DX12Local
{
    struct ResourceData;
}

class GraphicsBackendDX12 : public GraphicsBackendBase
{
public:
    void Init(void* data) override;
    GraphicsBackendName GetName() override;
    void InitNewFrame() override;
    void WaitForPreviousFrame() override;
    void FillImGuiInitData(void* data) override;
    void FillImGuiFrameData(void* data) override;

    GraphicsBackendTexture CreateTexture(TextureType type, const GraphicsBackendTextureDescriptor& descriptor, const std::string& name) override;
    GraphicsBackendSampler CreateSampler(const GraphicsBackendSamplerDescriptor& descriptor, const std::string& name) override;

    void GenerateMipmaps(const GraphicsBackendTexture &texture) override;
    void UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData) override;

    void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) override;
    TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear) override;

    GraphicsBackendBuffer CreateBuffer(const GraphicsBackendBufferDescriptor& descriptor, const std::string& name, const void* data = nullptr) override;
    GraphicsBackendBufferView CreateBufferView(const GraphicsBackendBufferViewDescriptor& descriptor, const GraphicsBackendBuffer& buffer, const std::string& name) override;

    void SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size) override;
    uint64_t GetMaxConstantBufferSize() override;
    int GetConstantBufferOffsetAlignment() override;

    GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, IndicesDataType indicesDataType, const std::string& name) override;

    void SetViewport(int x, int y, int width, int height, float near, float far) override;
    void SetScissorRect(int x, int y, int width, int height) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) override;
    GraphicsBackendShaderObject CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name) override;
    GraphicsBackendProgram CreateProgram(const GraphicsBackendProgramDescriptor& descriptor) override;
    void UseProgram(const GraphicsBackendProgram& program) override;

    void SetClearColor(float r, float g, float b, float a) override;
    void SetClearDepth(double depth) override;
    void SetStencilValue(uint8_t value) override;

    void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) override;
    void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) override;
    void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) override;
    void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) override;

    void Dispatch(uint32_t x, uint32_t y, uint32_t z) override;

    void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) override;

    void PushDebugGroup(const std::string& name, GPUQueue queue) override;
    void PopDebugGroup(GPUQueue queue) override;
    GraphicsBackendProfilerMarker PushProfilerMarker(GPUQueue queue) override;
    void PopProfilerMarker(GraphicsBackendProfilerMarker& marker) override;
    bool ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResult& outResult) override;

    void BeginRenderPass(const std::string& name) override;
    void EndRenderPass() override;
    void BeginCopyPass(const std::string& name) override;
    void EndCopyPass() override;
    void BeginComputePass(const std::string& name) override;
    void EndComputePass() override;

    GraphicsBackendFence CreateFence(FenceType fenceType, const std::string& name) override;
    void DeleteFence(const GraphicsBackendFence& fence) override;
    void SignalFence(const GraphicsBackendFence& fence) override;
    void WaitForFence(const GraphicsBackendFence& fence) override;

    void Flush() override;
    void Present() override;

    void TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor, ResourceState state, GPUQueue queue) override;
    void TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue) override;
    void TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue) override;

    bool RequireVertexAttributesForPSO() const override;
    bool RequirePrimitiveTypeForPSO() const override;
    bool RequireRTFormatsForPSO() const override;
    bool RequireStencilStateForPSO() const override;
    bool RequireDepthStateForPSO() const override;
    bool RequireRasterizerStateForPSO() const override;
    bool RequireBlendStateForPSO() const override;

    bool SupportsRaytracing() const override;
	GraphicsBackendBLAS CreateBLAS(const GraphicsBackendBLASDescriptor& descriptor, const std::string& name) override;
    GraphicsBackendTLAS CreateTLAS(const std::vector<GraphicsBackendRaytracingInstanceDescriptor>& instanceDescriptors, const std::string& name) override;

protected:
    void DeleteTexture_Internal(const GraphicsBackendTexture &texture) override;
    void DeleteSampler_Internal(const GraphicsBackendSampler &sampler) override;
    void DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer) override;
    void DeleteBufferView_Internal(const GraphicsBackendBufferView& bufferView) override;
    void DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry) override;
    void DeleteShader_Internal(GraphicsBackendShaderObject shader) override;
    void DeleteProgram_Internal(GraphicsBackendProgram program) override;
    void DeleteBLAS_Internal(GraphicsBackendBLAS& blas) override;
    void DeleteTLAS_Internal(GraphicsBackendTLAS& tlas) override;

    void BindTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index) override;
    void BindRWTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index) override;
    void BindSampler_Internal(const GraphicsBackendSampler& sampler, uint32_t index) override;
    void BindBuffer_Internal(const GraphicsBackendBufferView& bufferView, uint32_t index) override;
    void BindConstantBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size) override;
    void BindRWBuffer_Internal(const GraphicsBackendBufferView& bufferView, uint32_t index) override;

private:
    void BindResources(ProgramType programType);
    DX12Local::ResourceData* CreateBufferInternal(const GraphicsBackendBufferDescriptor& descriptor, ResourceState state, const std::string& name, const void* data) const;

    bool m_CopyTimestampSupported = false;
    uint32_t m_RaytracingTier = 0;
};

#endif // RENDER_BACKEND_DX12
#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_DX12_H
