#ifndef OPENGL_STUDY_GRAPHICS_H
#define OPENGL_STUDY_GRAPHICS_H

#include <string>

struct Matrix4x4;

namespace Graphics
{
    constexpr int OPENGL_MAJOR_VERSION = 4;
    constexpr int OPENGL_MINOR_VERSION =
#ifdef OPENGL_STUDY_WINDOWS
            6;
#elif OPENGL_STUDY_MACOS
            1;
#endif

    void               Init();
    void               Render();
    void               Reshape(int _width, int _height);
    const std::string &GetGlobalShaderDirectives();
    void               SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix);
    int                GetScreenWidth();
    int                GetScreenHeight();
}; // namespace Graphics

#endif //OPENGL_STUDY_GRAPHICS_H
