#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H

#include "graphics_backend.h"
#include "enums/graphics_backend_name.h"
#include "types/graphics_backend_vertex_attribute_descriptor.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#define FORWARD_DECLARE_ENUM(name) enum class name : GRAPHICS_BACKEND_TYPE_ENUM;
enum class TextureType;
enum class TextureInternalFormat;
enum class FramebufferAttachment;
FORWARD_DECLARE_ENUM(FramebufferTarget)
enum class PrimitiveType;
enum class BufferBindTarget;
enum class VertexAttributeDataType;
enum class BufferUsageHint;
FORWARD_DECLARE_ENUM(GraphicsBackendCapability)
FORWARD_DECLARE_ENUM(BlendFactor)
FORWARD_DECLARE_ENUM(CullFace)
FORWARD_DECLARE_ENUM(DepthFunction)
enum class ShaderType;
FORWARD_DECLARE_ENUM(UniformDataType)
FORWARD_DECLARE_ENUM(ClearMask)
FORWARD_DECLARE_ENUM(CullFaceOrientation)
enum class IndicesDataType;
FORWARD_DECLARE_ENUM(ProgramResourceParameter)
FORWARD_DECLARE_ENUM(BlitFramebufferMask)
FORWARD_DECLARE_ENUM(BlitFramebufferFilter)
enum class TextureWrapMode;
enum class TextureFilteringMode;
#undef FORWARD_DECLARE_ENUM

class GraphicsBackendTexture;
class GraphicsBackendSampler;
class GraphicsBackendBuffer;
class GraphicsBackendFramebuffer;
class GraphicsBackendProgram;
class GraphicsBackendShaderObject;
class GraphicsBackendGeometry;
class GraphicsBackendUniformInfo;
class GraphicsBackendBufferInfo;
struct GraphicsBackendResourceBindings;

class GraphicsBackendBase
{
public:
    static GraphicsBackendBase *Create(const std::string &backend);

    virtual void Init(void *device) = 0;
    virtual int GetMajorVersion() = 0;
    virtual int GetMinorVersion() = 0;
    virtual const std::string &GetShadingLanguageDirective() = 0;
    virtual GraphicsBackendName GetName() = 0;
    virtual void PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor) = 0;

    virtual GraphicsBackendTexture CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels, bool isRenderTarget) = 0;
    virtual GraphicsBackendSampler CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod) = 0;
    virtual void DeleteTexture(const GraphicsBackendTexture &texture) = 0;
    virtual void DeleteSampler(const GraphicsBackendSampler &sampler) = 0;

    virtual void BindTexture(const GraphicsBackendResourceBindings &bindings, int uniformLocation, const GraphicsBackendTexture &texture) = 0;
    virtual void BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler) = 0;

    virtual void GenerateMipmaps(const GraphicsBackendTexture &texture) = 0;

    virtual void UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData) = 0;
    virtual void DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels) = 0;
    virtual TextureInternalFormat GetTextureFormat(const GraphicsBackendTexture &texture) = 0;
    virtual int GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice) = 0;

    virtual void BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer) = 0;
    virtual void AttachTexture(FramebufferAttachment attachment, const GraphicsBackendTexture &texture, int level, int layer) = 0;
    virtual void AttachBackbuffer() = 0;

    virtual GraphicsBackendBuffer CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint) = 0;
    virtual void DeleteBuffer(const GraphicsBackendBuffer &buffer) = 0;
    virtual void BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer) = 0;
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

    virtual void SetCapability(GraphicsBackendCapability capability, bool enabled) = 0;

    virtual void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor) = 0;
    virtual void SetCullFace(CullFace cullFace) = 0;
    virtual void SetCullFaceOrientation(CullFaceOrientation orientation) = 0;
    virtual void SetDepthFunction(DepthFunction function) = 0;
    virtual void SetDepthWrite(bool enabled) = 0;
    virtual void SetDepthRange(double near, double far) = 0;
    virtual void SetViewport(int x, int y, int width, int height) = 0;

    virtual GraphicsBackendShaderObject CompileShader(ShaderType shaderType, const std::string &source) = 0;
    virtual GraphicsBackendProgram CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, TextureInternalFormat colorFormat, TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes) = 0;
    virtual void DeleteProgram(GraphicsBackendProgram program) = 0;
    virtual void UseProgram(GraphicsBackendProgram program) = 0;
    virtual void SetUniform(int location, UniformDataType dataType, int count, const void *data, bool transpose = false) = 0;
    virtual void IntrospectProgram(GraphicsBackendProgram program, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers) = 0;

    virtual void Clear(ClearMask mask) = 0;
    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void SetClearDepth(double depth) = 0;

    virtual void DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount) = 0;
    virtual void DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount) = 0;
    virtual void DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType) = 0;
    virtual void DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount) = 0;

    virtual bool SupportShaderStorageBuffer() = 0;

    virtual void BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter) = 0;

    virtual void PushDebugGroup(const std::string& name, int id) = 0;
    virtual void PopDebugGroup() = 0;

    virtual void BeginRenderPass() = 0;
    virtual void EndRenderPass() = 0;

    virtual GRAPHICS_BACKEND_TYPE_ENUM GetError() = 0;
    virtual const char *GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error) = 0;

    bool IsCompressedTextureFormat(TextureInternalFormat format);
    int GetBlockSize(TextureInternalFormat format);
    int GetBlockBytes(TextureInternalFormat format);
};


#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BASE_H
