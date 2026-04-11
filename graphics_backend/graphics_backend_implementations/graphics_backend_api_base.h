#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H

#include "enums/graphics_backend_name.h"
#include "enums/buffer_type.h"
#include "enums/texture_internal_format.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"
#include "types/graphics_backend_profiler_marker.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "types/graphics_backend_depth_descriptor.h"
#include "types/graphics_backend_rasterizer_descriptor.h"
#include "types/graphics_backend_blend_descriptor.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_blas.h"
#include "types/graphics_backend_tlas.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>

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
class GraphicsBackendShaderObject;
class GraphicsBackendGeometry;
struct GraphicsBackendTextureInfo;
class GraphicsBackendBufferInfo;
struct GraphicsBackendRenderTargetDescriptor;
struct GraphicsBackendColorAttachmentDescriptor;
struct GraphicsBackendFence;
struct GraphicsBackendSamplerInfo;
struct GraphicsBackendProgramDescriptor;
struct GraphicsBackendTextureDescriptor;
struct GraphicsBackendSamplerDescriptor;
struct GraphicsBackendBufferDescriptor;
struct GraphicsBackendBufferViewDescriptor;
struct GraphicsBackendBufferView;
struct GraphicsBackendRaytracingInstanceDescriptor;
struct GraphicsBackendBLASDescriptor;

class GraphicsBackendBase
{
public:
	virtual ~GraphicsBackendBase() = default;

	static GraphicsBackendBase *Create();

    virtual void Init(void *data);
    virtual GraphicsBackendName GetName() = 0;
    virtual void InitNewFrame();
    virtual void WaitForPreviousFrame() = 0;
    virtual void FillImGuiInitData(void* data) = 0;
    virtual void FillImGuiFrameData(void* data) = 0;

    void IncrementFrameNumber();
    uint64_t GetFrameNumber() const;

    virtual GraphicsBackendTexture CreateTexture(TextureType type, const GraphicsBackendTextureDescriptor& descriptor, const std::string& name) = 0;
    virtual GraphicsBackendSampler CreateSampler(const GraphicsBackendSamplerDescriptor& descriptor, const std::string& name) = 0;
    void DeleteTexture(const GraphicsBackendTexture& texture);
    void DeleteSampler(const GraphicsBackendSampler& sampler);

    void BindTexture(const GraphicsBackendTexture& texture, uint32_t index);
    void BindSampler(const GraphicsBackendSampler& sampler, uint32_t index);
    void BindTextureSampler(const GraphicsBackendTexture& texture, const GraphicsBackendSampler& sampler, uint32_t index);
    void BindRWTexture(const GraphicsBackendTexture& texture, uint32_t index);

    virtual void GenerateMipmaps(const GraphicsBackendTexture &texture) = 0;
    virtual void UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData) = 0;
    void UploadImagePixels(const GraphicsBackendTexture& texture, int level, int width, int height, int depth, int imageSize, const void* pixelsData);

    virtual void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) = 0;
    virtual TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment, bool *outIsLinear) = 0;

    virtual GraphicsBackendBuffer CreateBuffer(const GraphicsBackendBufferDescriptor& descriptor, const std::string& name, const void* data = nullptr) = 0;
    virtual GraphicsBackendBufferView CreateBufferView(const GraphicsBackendBufferViewDescriptor& descriptor, const GraphicsBackendBuffer& buffer, const std::string& name) = 0;
    void DeleteBuffer(const GraphicsBackendBuffer& buffer);
    void DeleteBufferView(const GraphicsBackendBufferView& bufferView);
    void BindBuffer(const GraphicsBackendBufferView& bufferView, uint32_t index);
    void BindConstantBuffer(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size);
    void BindRWBuffer(const GraphicsBackendBufferView& bufferView, uint32_t index);

    virtual void SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data) = 0;
    virtual void CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size) = 0;
    virtual uint64_t GetMaxConstantBufferSize() = 0;
    virtual int GetConstantBufferOffsetAlignment() = 0;

    virtual GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer& vertexBuffer, const GraphicsBackendBuffer& indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes, IndicesDataType indicesDataType, const std::string& name) = 0;
    void DeleteGeometry(const GraphicsBackendGeometry &geometry);

    virtual void SetViewport(int x, int y, int width, int height, float near, float far) = 0;
    virtual void SetScissorRect(int x, int y, int width, int height) = 0;

    virtual GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source, const std::string& name) = 0;
    virtual GraphicsBackendShaderObject CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name) = 0;
    virtual GraphicsBackendProgram CreateProgram(const GraphicsBackendProgramDescriptor& descriptor) = 0;
    void DeleteShader(GraphicsBackendShaderObject shader);
    void DeleteProgram(GraphicsBackendProgram program);
    virtual void UseProgram(const GraphicsBackendProgram& program);

    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void SetClearDepth(double depth) = 0;
    virtual void SetStencilValue(uint8_t value) = 0;

    virtual void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) = 0;
    virtual void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) = 0;
    virtual void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) = 0;
    virtual void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) = 0;

    virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;

    virtual void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) = 0;

    virtual void PushDebugGroup(const std::string& name, GPUQueue queue) = 0;
    virtual void PopDebugGroup(GPUQueue queue) = 0;
    virtual GraphicsBackendProfilerMarker PushProfilerMarker(GPUQueue queue) = 0;
    virtual void PopProfilerMarker(GraphicsBackendProfilerMarker& marker) = 0;
    virtual bool ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResult& outResult) = 0;

    virtual void BeginRenderPass(const std::string& name) = 0;
    virtual void EndRenderPass();
    virtual void BeginCopyPass(const std::string& name) = 0;
    virtual void EndCopyPass() = 0;
    virtual void BeginComputePass(const std::string& name) = 0;
    virtual void EndComputePass() = 0;

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

    void SetDepthState(const GraphicsBackendDepthDescriptor& depthDescriptor);
    const GraphicsBackendDepthDescriptor& GetDepthState() const;

    void SetRasterizerState(const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor);
    const GraphicsBackendRasterizerDescriptor& GetRasterizerState() const;

    void SetBlendState(const GraphicsBackendBlendDescriptor& blendDescriptor);
    const GraphicsBackendBlendDescriptor& GetBlendState() const;

    virtual bool RequireVertexAttributesForPSO() const = 0;
    virtual bool RequirePrimitiveTypeForPSO() const = 0;
    virtual bool RequireRTFormatsForPSO() const = 0;
    virtual bool RequireStencilStateForPSO() const = 0;
    virtual bool RequireDepthStateForPSO() const = 0;
    virtual bool RequireRasterizerStateForPSO() const = 0;
    virtual bool RequireBlendStateForPSO() const = 0;

    bool IsTexture3D(TextureType type);
    bool IsCompressedTextureFormat(TextureInternalFormat format);
    int GetBlockSize(TextureInternalFormat format);
    int GetBlockBytes(TextureInternalFormat format);
    bool IsDepthFormat(TextureInternalFormat format);
    bool IsDepthAttachment(FramebufferAttachment attachment);
    uint32_t GetFormatSize(TextureInternalFormat format);

    uint32_t GetDrawCallCount() const
    {
        return m_DrawCallCount;
    }

    virtual bool SupportsRaytracing() const;
    virtual GraphicsBackendBLAS CreateBLAS(const GraphicsBackendBLASDescriptor& descriptor, const std::string& name);
    virtual GraphicsBackendTLAS CreateTLAS(const std::vector<GraphicsBackendRaytracingInstanceDescriptor>& instanceDescriptors, const std::string& name);
    void DeleteBLAS(GraphicsBackendBLAS& BLAS);
    void DeleteTLAS(GraphicsBackendTLAS& TLAS);

    static size_t GetDepthDescriptorHash(const GraphicsBackendDepthDescriptor& depthDescriptor);
    static size_t GetStencilOperationDescriptorHash(const GraphicsBackendStencilOperationDescriptor& stencilOperationDescriptor);
    static size_t GetStencilDescriptorHash(const GraphicsBackendStencilDescriptor& stencilDescriptor);
    static size_t GetRasterizerDescriptorHash(const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor);
    static size_t GetBlendDescriptorHash(const GraphicsBackendBlendDescriptor& blendDescriptor);

    static std::string GetShaderTypeName(ShaderType shaderType);

protected:
    bool IsMainThread() const;
    bool IsBoundResourcesDirty() const;
    void BindResources();
    void DeleteResources();

    virtual void DeleteTexture_Internal(const GraphicsBackendTexture &texture) = 0;
    virtual void DeleteSampler_Internal(const GraphicsBackendSampler &sampler) = 0;
    virtual void DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer) = 0;
    virtual void DeleteBufferView_Internal(const GraphicsBackendBufferView& bufferView) = 0;
    virtual void DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry) = 0;
    virtual void DeleteShader_Internal(GraphicsBackendShaderObject shader) = 0;
    virtual void DeleteProgram_Internal(GraphicsBackendProgram program) = 0;
    virtual void DeleteBLAS_Internal(GraphicsBackendBLAS& blas){}
    virtual void DeleteTLAS_Internal(GraphicsBackendTLAS& tlas){}

    virtual void BindTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index) = 0;
    virtual void BindRWTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index) = 0;
    virtual void BindSampler_Internal(const GraphicsBackendSampler& sampler, uint32_t index) = 0;
    virtual void BindBuffer_Internal(const GraphicsBackendBufferView& bufferView, uint32_t index) = 0;
    virtual void BindConstantBuffer_Internal(const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size) = 0;
    virtual void BindRWBuffer_Internal(const GraphicsBackendBufferView& bufferView, uint32_t index) = 0;

	static GraphicsBackendProgram CreateProgram(uint64_t programPtr, const GraphicsBackendProgramDescriptor& descriptor);

    GraphicsBackendProgram m_CurrentProgram{};
    uint32_t m_DrawCallCount = 0;

private:
    struct BufferBindInfo
    {
        GraphicsBackendBuffer Buffer;
        BufferType Type;
        int Offset;
        int Size;
        int ElementsCount = 0;
        TextureInternalFormat Format = TextureInternalFormat::INVALID;
    };

    uint64_t m_FrameCount = 0;
    std::thread::id m_MainThreadId;

    std::vector<GraphicsBackendTexture> m_DeletedTextures;
    std::vector<GraphicsBackendSampler> m_DeletedSamplers;
    std::vector<GraphicsBackendBuffer> m_DeletedBuffers;
    std::vector<GraphicsBackendBufferView> m_DeletedBufferViews;
    std::vector<GraphicsBackendGeometry> m_DeletedGeometries;
    std::vector<GraphicsBackendShaderObject> m_DeletedShaders;
    std::vector<GraphicsBackendProgram> m_DeletedPrograms;
    std::vector<GraphicsBackendBLAS> m_DeletedBLASes;
    std::vector<GraphicsBackendTLAS> m_DeletedTLASes;

    std::mutex m_DeletedTexturesMutex;
    std::mutex m_DeletedSamplersMutex;
    std::mutex m_DeletedBuffersMutex;
    std::mutex m_DeletedBufferViewsMutex;
    std::mutex m_DeletedGeometriesMutex;
    std::mutex m_DeletedShadersMutex;
    std::mutex m_DeletedProgramsMutex;
    std::mutex m_DeletedBLASesMutex;
    std::mutex m_DeletedTLASesMutex;

    std::unordered_map<uint32_t, GraphicsBackendTexture> m_BoundTextures;
    std::unordered_map<uint32_t, GraphicsBackendTexture> m_BoundRWTextures;
    std::unordered_map<uint32_t, GraphicsBackendSampler> m_BoundSamplers;
    std::unordered_map<uint32_t, GraphicsBackendBufferView> m_BoundBuffers;
    std::unordered_map<uint32_t, BufferBindInfo> m_BoundConstantBuffers;
    std::unordered_map<uint32_t, GraphicsBackendBufferView> m_BoundRWBuffers;

    uint32_t m_BoundTexturesDirtyMask = 0;
    uint32_t m_BoundRWTexturesDirtyMask = 0;
    uint32_t m_BoundSamplersDirtyMask = 0;
    uint32_t m_BoundBuffersDirtyMask = 0;
    uint32_t m_BoundConstantBuffersDirtyMask = 0;
    uint32_t m_BoundRWBuffersDirtyMask = 0;

    GraphicsBackendStencilDescriptor m_StencilDescriptor;
    GraphicsBackendDepthDescriptor m_DepthDescriptor;
    GraphicsBackendRasterizerDescriptor m_RasterizerDescriptor;
    GraphicsBackendBlendDescriptor m_BlendDescriptor;
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
