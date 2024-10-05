#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H

#include "enums/graphics_backend_name.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

enum class TextureType;
enum class TextureInternalFormat;
enum class FramebufferAttachment;
enum class PrimitiveType;
enum class BufferBindTarget;
enum class VertexAttributeDataType;
enum class BufferUsageHint;
enum class BlendFactor;
enum class CullFace;
enum class DepthFunction;
enum class ShaderType;
enum class TextureDataType;
enum class CullFaceOrientation;
enum class IndicesDataType;
enum class TextureWrapMode;
enum class TextureFilteringMode;

class GraphicsBackendTexture;
class GraphicsBackendSampler;
class GraphicsBackendBuffer;
class GraphicsBackendProgram;
class GraphicsBackendShaderObject;
class GraphicsBackendGeometry;
struct GraphicsBackendTextureInfo;
class GraphicsBackendBufferInfo;
struct GraphicsBackendResourceBindings;
struct GraphicsBackendRenderTargetDescriptor;
struct GraphicsBackendDepthStencilState;
struct GraphicsBackendColorAttachmentDescriptor;

class GraphicsBackendBase
{
public:
    static GraphicsBackendBase *Create(const std::string &backend);

    virtual void Init(void *device) = 0;
    virtual const std::string &GetShadingLanguageDirective() = 0;
    virtual GraphicsBackendName GetName() = 0;
    virtual void PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor) = 0;

    virtual GraphicsBackendTexture CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels, bool isRenderTarget) = 0;
    virtual GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod) = 0;
    virtual void DeleteTexture(const GraphicsBackendTexture &texture) = 0;
    virtual void DeleteSampler(const GraphicsBackendSampler &sampler) = 0;

    virtual void BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture) = 0;
    virtual void BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler) = 0;

    virtual void GenerateMipmaps(const GraphicsBackendTexture &texture) = 0;

    virtual void UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData) = 0;
    virtual void DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels) = 0;
    virtual TextureInternalFormat GetTextureFormat(const GraphicsBackendTexture &texture) = 0;
    virtual int GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice) = 0;

    virtual void AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor) = 0;
    virtual TextureInternalFormat GetRenderTargetFormat(FramebufferAttachment attachment) = 0;

    virtual GraphicsBackendBuffer CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint) = 0;
    virtual void DeleteBuffer(const GraphicsBackendBuffer &buffer) = 0;
    virtual void BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size) = 0;

    virtual void SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data) = 0;
    virtual void CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size) = 0;
    virtual uint64_t GetMaxConstantBufferSize() = 0;
    virtual int GetConstantBufferOffsetAlignment() = 0;

    virtual GraphicsBackendGeometry CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes) = 0;
    virtual void DeleteGeometry(const GraphicsBackendGeometry &geometry) = 0;

    virtual void EnableVertexAttributeArray(int index) = 0;
    virtual void DisableVertexAttributeArray(int index) = 0;
    virtual void SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer) = 0;
    virtual void SetVertexAttributeDivisor(int index, int divisor) = 0;

    virtual void SetCullFace(CullFace cullFace) = 0;
    virtual void SetCullFaceOrientation(CullFaceOrientation orientation) = 0;
    virtual void SetViewport(int x, int y, int width, int height, float near, float far) = 0;

    virtual GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source) = 0;
    virtual GraphicsBackendProgram CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                                                 std::unordered_map<std::string, GraphicsBackendTextureInfo>* textures, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>* buffers) = 0;
    virtual void DeleteShader(GraphicsBackendShaderObject shader) = 0;
    virtual void DeleteProgram(GraphicsBackendProgram program) = 0;
    virtual void UseProgram(GraphicsBackendProgram program) = 0;
    virtual bool RequireStrictPSODescriptor() = 0;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void SetClearDepth(double depth) = 0;

    virtual void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) = 0;
    virtual void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) = 0;
    virtual void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) = 0;
    virtual void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) = 0;

    virtual bool SupportShaderStorageBuffer() = 0;

    virtual void CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height) = 0;

    virtual void PushDebugGroup(const std::string& name, int id) = 0;
    virtual void PopDebugGroup() = 0;

    virtual void BeginRenderPass() = 0;
    virtual void EndRenderPass() = 0;

    virtual GraphicsBackendDepthStencilState CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction) = 0;
    virtual void DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state) = 0;
    virtual void SetDepthStencilState(const GraphicsBackendDepthStencilState& state) = 0;

    bool IsCompressedTextureFormat(TextureInternalFormat format);
    int GetBlockSize(TextureInternalFormat format);
    int GetBlockBytes(TextureInternalFormat format);
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
