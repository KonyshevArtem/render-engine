#ifndef RENDER_ENGINE_GRAPHICS_H
#define RENDER_ENGINE_GRAPHICS_H

#include <memory>
#include <string>
#include <vector>

struct Vector4;
struct Matrix4x4;

namespace Graphics
{
    void Init();
    void Shutdown();
    void Render(int width, int height);
    void SetCameraData(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float nearPlane, float farPlane);
    int GetScreenWidth();
    int GetScreenHeight();
    Matrix4x4 GetGPUProjectionMatrix(const Matrix4x4& projectionMatrix);
}; // namespace Graphics

#endif //RENDER_ENGINE_GRAPHICS_H
