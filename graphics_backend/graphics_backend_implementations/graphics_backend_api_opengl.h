#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H

#include "graphics_backend_api_base.h"
#include <set>

class GraphicsBackendOpenGL : public GraphicsBackendBase
{
public:
    void Init(void *data) override;

    const std::string &GetGLSLVersionString();
    GraphicsBackendName GetName() override;
    void InitNewFrame(void *data) override;

    GraphicsBackendTexture CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels, bool isRenderTarget) override;
    GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod) override;
    void DeleteTexture(const GraphicsBackendTexture &texture) override;
    void DeleteSampler(const GraphicsBackendSampler &sampler) override;

    void BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture) override;
    void BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler) override;

    void GenerateMipmaps(const GraphicsBackendTexture &texture) override;

    void UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData) override;
    void DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels) override;
    TextureInternalFormat GetTextureFormat(const GraphicsBackendTexture &texture) override;
    int GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice) override;

    void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) override;
    TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment) override;

    GraphicsBackendBuffer CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint) override;
    void DeleteBuffer(const GraphicsBackendBuffer &buffer) override;
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

    void SetCullFace(CullFace cullFace) override;
    void SetCullFaceOrientation(CullFaceOrientation orientation) override;
    void SetViewport(int x, int y, int width, int height, float near, float far) override;

    GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source) override;
    GraphicsBackendProgram CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes) override;
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

    bool SupportShaderStorageBuffer() override;

    void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) override;

    void PushDebugGroup(const std::string& name, int id) override;
    void PopDebugGroup() override;

    void BeginRenderPass() override;
    void EndRenderPass() override;

    GraphicsBackendDepthStencilState CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction) override;
    void DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state) override;
    void SetDepthStencilState(const GraphicsBackendDepthStencilState& state) override;

private:
    std::set<std::string> m_Extensions;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_OPENGL_H
