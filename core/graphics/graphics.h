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
class RenderQueue;
struct GraphicsBackendRenderTargetDescriptor;

namespace Graphics
{
    void Init();
    void Shutdown();
    void Render(int width, int height);
    void DrawRenderQueue(const RenderQueue& renderQueue);
    void Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix);
    void DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices);
    void SetCameraData(const Matrix4x4& viewMatrix, Matrix4x4 projectionMatrix, float nearPlane, float farPlane);
    int  GetScreenWidth();
    int  GetScreenHeight();
    void SetRenderTarget(GraphicsBackendRenderTargetDescriptor descriptor, const std::shared_ptr<Texture> &target = nullptr);
    void SetViewport(const Vector4& viewport);
    void CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size);
    void Blit(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, Material &material, const std::string &name);
    void CopyTextureToTexture(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, GraphicsBackendRenderTargetDescriptor destinationDescriptor);

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture);

    int GetDrawCallCount();
}; // namespace Graphics

#endif //RENDER_ENGINE_GRAPHICS_H
