#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include <string>

struct Matrix4x4;
struct Vector4;
class RenderPass;
class ShadowCasterPass;
class SkyboxPass;
class UniformBlock;
class Light;

namespace Graphics
{
    void               Init(int _argc, char **_argv);
    void               Render();
    void               Reshape(int _width, int _height);
    const std::string &GetGlobalShaderDirectives();
    void               SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix);
    int                GetScreenWidth();
    int                GetScreenHeight();
}; // namespace Graphics

#endif //OPENGL_STUDY_GRAPHICS_H
