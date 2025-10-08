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
    void SetCameraData(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float nearPlane, float farPlane);
    int  GetScreenWidth();
    int  GetScreenHeight();
    void SetViewport(const Vector4& viewport);
    Matrix4x4 GetGPUProjectionMatrix(const Matrix4x4& projectionMatrix);

    int GetDrawCallCount();
}; // namespace Graphics

#endif //RENDER_ENGINE_GRAPHICS_H
