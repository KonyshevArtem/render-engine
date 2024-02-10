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
struct DrawCallInfo;
struct RenderSettings;
class Renderer;
class Texture;

namespace Graphics
{
    constexpr int MAX_SPOT_LIGHT_SOURCES = 3;
    constexpr int MAX_POINT_LIGHT_SOURCES = 3;

    void                      Init();
    void                      Shutdown();
    void                      Render();
    std::vector<DrawCallInfo> DoCulling(const std::vector<Renderer *> &_renderers);
    void                      Draw(const std::vector<DrawCallInfo> &_drawCallInfos, const RenderSettings &_settings);
    void                      Reshape(int _width, int _height);
    const std::string        &GetGlobalShaderDirectives();
    void                      SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix);
    int                       GetScreenWidth();
    int                       GetScreenHeight();
    void                      SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, int colorLevel, int colorLayer,
                                               const std::shared_ptr<Texture> &_depthAttachment, int depthLevel, int depthLayer);
    void                      SetViewport(const Vector4& viewport);

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture);
    void SetGlobalMatrix(const std::string &name, const Matrix4x4 &matrix);

}; // namespace Graphics

#endif //OPENGL_STUDY_GRAPHICS_H
