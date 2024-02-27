#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include "enums/depth_function.h"
#include "enums/blend_factor.h"
#include "enums/cull_face.h"

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

namespace Graphics
{
    constexpr int MAX_SPOT_LIGHT_SOURCES = 3;
    constexpr int MAX_POINT_LIGHT_SOURCES = 3;

    void                      Init();
    void                      Shutdown();
    void                      Render();
    void                      DrawRenderers(const std::vector<std::shared_ptr<Renderer>> &renderers, const RenderSettings &settings);
    void                      Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix, int shaderPassIndex);
    void                      DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices, int shaderPassIndex);
    void                      Reshape(int _width, int _height);
    const std::string        &GetGlobalShaderDirectives();
    void                      SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix);
    int                       GetScreenWidth();
    int                       GetScreenHeight();
    void                      SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, int colorLevel, int colorLayer,
                                               const std::shared_ptr<Texture> &_depthAttachment, int depthLevel, int depthLayer);
    void                      SetViewport(const Vector4& viewport);
    void                      CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size);

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture);

}; // namespace Graphics

#endif //OPENGL_STUDY_GRAPHICS_H
