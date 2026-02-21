#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H

#include "enums/graphics_backend_name.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"
#include "types/graphics_backend_profiler_marker.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_stencil_descriptor.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>

enum class TextureType;
enum class TextureInternalFormat : uint16_t;
enum class FramebufferAttachment;
enum class PrimitiveType;
enum class VertexAttributeDataType;
enum class BlendFactor;
enum class CullFace;
enum class ComparisonFunction;
enum class ShaderType;
enum class TextureDataType;
enum class CullFaceOrientation;
enum class IndicesDataType;
enum class TextureWrapMode;
enum class TextureFilteringMode;
enum class CubemapFace;
enum class FenceType;
enum class ResourceState : uint64_t;

class GraphicsBackendTexture;
class GraphicsBackendSampler;
class GraphicsBackendProgram;
class GraphicsBackendShaderObject;
class GraphicsBackendGeometry;
struct GraphicsBackendTextureInfo;
class GraphicsBackendBufferInfo;
struct GraphicsBackendRenderTargetDescriptor;
struct GraphicsBackendColorAttachmentDescriptor;
struct GraphicsBackendFence;
struct GraphicsBackendSamplerInfo;
struct GraphicsBackendProgramDescriptor;

class GraphicsBackendBase
{
public:
	virtual ~GraphicsBackendBase() = default;

	static GraphicsBackendBase *Create();

    virtual void Init(void *data);
    virtual GraphicsBackendName GetName() = 0;
    virtual void InitNewFrame();
    virtual void FillImGuiInitData(void* data) = 0;
    virtual void FillImGuiFrameData(void* data) = 0;

    void IncrementFrameNumber();
    uint64_t GetFrameNumber() const;

    virtual GraphicsBackendTexture CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name) = 0;
    virtual GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, ComparisonFunction comparisonFunction, const std::string& name) = 0;
    void DeleteTexture(const GraphicsBackendTexture& texture);
    void DeleteSampler(const GraphicsBackendSampler& sampler);

    void BindTexture(const GraphicsBackendTexture& texture, uint32_t index);
    void BindSampler(const GraphicsBackendSampler& sampler, uint32_t index);
    void BindTextureSampler(const GraphicsBackendTexture& texture, const GraphicsBackendSampler& sampler, uint32_t index);

    virtual void GenerateMipmaps(const GraphicsBackendTexture &texture) = 0;
    virtual void UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData) = 0;

    virtual void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) = 0;
    virtual TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment, bool *outIsLinear) = 0;

    virtual GraphicsBackendBuffer CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data = nullptr) = 0;
    void DeleteBuffer(const GraphicsBackendBuffer& buffer);
    void BindBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size);
    void BindStructuredBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size, int count);
    void BindConstantBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size);

    virtual void SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data) = 0;
    virtual void CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size) = 0;
    virtual uint64_t GetMaxConstantBufferSize() = 0;
    virtual int GetConstantBufferOffsetAlignment() = 0;

    virtual GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name) = 0;
    void DeleteGeometry(const GraphicsBackendGeometry &geometry);

    virtual void SetViewport(int x, int y, int width, int height, float near, float far) = 0;
    virtual void SetScissorRect(int x, int y, int width, int height) = 0;

    virtual GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) = 0;
    virtual GraphicsBackendShaderObject CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name) = 0;
    virtual GraphicsBackendProgram CreateProgram(const GraphicsBackendProgramDescriptor& descriptor) = 0;
    void DeleteShader(GraphicsBackendShaderObject shader);
    void DeleteProgram(GraphicsBackendProgram program);
    virtual void UseProgram(const GraphicsBackendProgram& program) = 0;
    virtual bool RequireStrictPSODescriptor() = 0;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void SetClearDepth(double depth) = 0;
    virtual void SetStencilValue(uint8_t value) = 0;

    virtual void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) = 0;
    virtual void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) = 0;
    virtual void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) = 0;
    virtual void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) = 0;

    virtual void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) = 0;

    virtual void PushDebugGroup(const std::string& name, GPUQueue queue) = 0;
    virtual void PopDebugGroup(GPUQueue queue) = 0;
    virtual GraphicsBackendProfilerMarker PushProfilerMarker() = 0;
    virtual void PopProfilerMarker(GraphicsBackendProfilerMarker& marker) = 0;
    virtual bool ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResults& outResults) = 0;

    virtual void BeginRenderPass(const std::string& name) = 0;
    virtual void EndRenderPass() = 0;
    virtual void BeginCopyPass(const std::string& name) = 0;
    virtual void EndCopyPass() = 0;

    virtual GraphicsBackendFence CreateFence(FenceType fenceType, const std::string& name) = 0;
    virtual void DeleteFence(const GraphicsBackendFence& fence) = 0;
    virtual void SignalFence(const GraphicsBackendFence& fence) = 0;
    virtual void WaitForFence(const GraphicsBackendFence& fence) = 0;

    virtual void Flush() = 0;
    virtual void Present() = 0;

    virtual void TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor& descriptor, ResourceState state, GPUQueue queue) = 0;
    virtual void TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue) = 0;
    virtual void TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue) = 0;

    void SetStencilState(const GraphicsBackendStencilDescriptor& stencilDescriptor);
    const GraphicsBackendStencilDescriptor& GetStencilDescriptor() const;

    bool IsTexture3D(TextureType type);
    bool IsCompressedTextureFormat(TextureInternalFormat format);
    int GetBlockSize(TextureInternalFormat format);
    int GetBlockBytes(TextureInternalFormat format);
    bool IsDepthFormat(TextureInternalFormat format);
    bool IsDepthAttachment(FramebufferAttachment attachment);

    uint32_t GetDrawCallCount()
    {
        return m_DrawCallCount;
    }

protected:
    bool IsMainThread();

    virtual void DeleteTexture_Internal(const GraphicsBackendTexture &texture) = 0;
    virtual void DeleteSampler_Internal(const GraphicsBackendSampler &sampler) = 0;
    virtual void DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer) = 0;
    virtual void DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry) = 0;
    virtual void DeleteShader_Internal(GraphicsBackendShaderObject shader) = 0;
    virtual void DeleteProgram_Internal(GraphicsBackendProgram program) = 0;

    void BindResources(const GraphicsBackendProgram& program);
    virtual void BindTexture_Internal(const GraphicsBackendTexture &texture, uint32_t index) = 0;
    virtual void BindSampler_Internal(const GraphicsBackendSampler &sampler, uint32_t index) = 0;
    virtual void BindBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size) = 0;
    virtual void BindStructuredBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size, int count) = 0;
    virtual void BindConstantBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size) = 0;

    GraphicsBackendProgram CreateProgram(uint64_t programPtr, const GraphicsBackendProgramDescriptor& descriptor);

    uint32_t m_DrawCallCount = 0;

private:
    struct BufferBindInfo
    {
        GraphicsBackendBuffer Buffer;
        int Offset;
        int Size;
        int ElementsCount;
    };

    uint64_t m_FrameCount = 0;
    std::thread::id m_MainThreadId;

    std::vector<std::pair<GraphicsBackendTexture, int>> m_DeletedTextures;
    std::vector<std::pair<GraphicsBackendSampler, int>> m_DeletedSamplers;
    std::vector<std::pair<GraphicsBackendBuffer, int>> m_DeletedBuffers;
    std::vector<std::pair<GraphicsBackendGeometry, int>> m_DeletedGeometries;
    std::vector<std::pair<GraphicsBackendShaderObject, int>> m_DeletedShaders;
    std::vector<std::pair<GraphicsBackendProgram, int>> m_DeletedPrograms;

    std::unordered_map<uint32_t, GraphicsBackendTexture> m_BoundTextures;
    std::unordered_map<uint32_t, GraphicsBackendSampler> m_BoundSamplers;
    std::unordered_map<uint32_t, BufferBindInfo> m_BoundBuffers;
    std::unordered_map<uint32_t, BufferBindInfo> m_BoundConstantBuffers;
    std::unordered_map<uint32_t, BufferBindInfo> m_BoundStructuredBuffers;

    GraphicsBackendStencilDescriptor m_StencilDescriptor;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
