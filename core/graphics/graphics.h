#ifndef RENDER_ENGINE_GRAPHICS_H
#define RENDER_ENGINE_GRAPHICS_H

#include <memory>
#include <string>
#include <vector>

struct Vector4;
struct Matrix4x4;
struct RenderSettings;
class Renderer;
class Texture;
class GraphicsBuffer;
class DrawableGeometry;
class Material;
struct GraphicsBackendRenderTargetDescriptor;

namespace Graphics
{
    void                      Init();
    void                      Shutdown();
    void                      Render(int width, int height);
    void                      DrawRenderers(const std::vector<std::shared_ptr<Renderer>> &renderers, const RenderSettings &settings);
    void                      Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData = nullptr, uint64_t perInstanceOffset = 0);
    void                      DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData = nullptr, uint64_t perInstanceDataOffset = 0, const std::shared_ptr<GraphicsBuffer> &perInstanceIndices = nullptr, uint64_t perInstanceIndicesOffset = 0);
    void                      SetCameraData(const Matrix4x4 &_viewMatrix, Matrix4x4 _projectionMatrix);
    int                       GetScreenWidth();
    int                       GetScreenHeight();
    void                      SetRenderTarget(GraphicsBackendRenderTargetDescriptor descriptor, const std::shared_ptr<Texture> &target = nullptr);
    void                      SetViewport(const Vector4& viewport);
    void                      CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size);
    void                      Blit(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, Material &material, const std::string &name);
    void                      CopyTextureToTexture(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, GraphicsBackendRenderTargetDescriptor destinationDescriptor);

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture);

}; // namespace Graphics

#endif //RENDER_ENGINE_GRAPHICS_H
