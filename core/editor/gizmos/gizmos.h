#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_GIZMOS_H
#define OPENGL_STUDY_GIZMOS_H

#include <vector>
#include <memory>

struct Matrix4x4;
class DrawableGeometry;

namespace Gizmos
{
    void DrawWireCube(const Matrix4x4 &_matrix);

    void Init();
    const std::vector<std::pair<std::shared_ptr<DrawableGeometry>, Matrix4x4>> &GetGizmosToDraw();
    void ClearGizmos();

    bool IsEnabled();
    void SetEnabled(bool enabled);
} // namespace Gizmos

#endif

#endif