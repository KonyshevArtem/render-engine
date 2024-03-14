#if RENDER_ENGINE_EDITOR

#ifndef RENDER_ENGINE_GIZMOS_H
#define RENDER_ENGINE_GIZMOS_H

#include <vector>
#include <unordered_map>
#include <memory>

struct Matrix4x4;
class DrawableGeometry;

namespace Gizmos
{
    void DrawWireCube(const Matrix4x4 &_matrix);

    void Init();
    const std::unordered_map<std::shared_ptr<DrawableGeometry>, std::vector<Matrix4x4>> &GetGizmosToDraw();
    void ClearGizmos();

    bool IsEnabled();
    void SetEnabled(bool enabled);
} // namespace Gizmos

#endif

#endif