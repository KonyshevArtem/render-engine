#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H

#ifdef RENDER_BACKEND_OPENGL

#include "graphics_backend_api_base.h"
#include <set>

class GraphicsBackendOpenGL : public GraphicsBackendBase
{
public:
    void Init(void* data) override;
    GraphicsBackendName GetName() override;
    void InitNewFrame() override;
    void FillImGuiInitData(void* data) override;
    void FillImGuiFrameData(void* data) override;

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

    GraphicsBackendBuffer CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data = nullptr) override;
    void DeleteBuffer(const GraphicsBackendBuffer &buffer) override;
    void BindBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;
    void BindStructuredBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int elementOffset, int elementSize, int elementCount) override;
    void BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) override;

    void SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data) override;
    void CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size) override;
    uint64_t GetMaxConstantBufferSize() override;
    int GetConstantBufferOffsetAlignment() override;

    GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name) override;
    void DeleteGeometry(const GraphicsBackendGeometry &geometry) override;

    void SetViewport(int x, int y, int width, int height, float near, float far) override;
    void SetScissorRect(int x, int y, int width, int height) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) override;
    GraphicsBackendShaderObject CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name) override;
    GraphicsBackendProgram CreateProgram(const GraphicsBackendProgramDescriptor& descriptor) override;
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

    void TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor& target, ResourceState state, GPUQueue queue) override;
    void TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue) override;
    void TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue) override;

private:
    std::set<std::string> m_Extensions;
};

#endif // RENDER_BACKEND_OPENGL
#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
