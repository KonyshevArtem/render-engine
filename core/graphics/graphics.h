#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include <memory>
#include <string>
#include <vector>

struct Matrix4x4;
struct DrawCallInfo;
struct RenderSettings;
class Renderer;
class Texture;

namespace Graphics
{
    constexpr int MAX_SPOT_LIGHT_SOURCES = 3;

    constexpr int OPENGL_MAJOR_VERSION = 4;
    constexpr int OPENGL_MINOR_VERSION =
#ifdef OPENGL_STUDY_WINDOWS
            6;
#elif OPENGL_STUDY_MACOS
            1;
#endif

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
    void                      SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, const std::shared_ptr<Texture> &_depthAttachment);
}; // namespace Graphics

#endif //OPENGL_STUDY_GRAPHICS_H
