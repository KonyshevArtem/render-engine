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
    enum class GizmoType
    {
        WIRE_CUBE,
        FRUSTUM
    };

    void DrawWireCube(const Matrix4x4& matrix);
    void DrawFrustum(const Matrix4x4& matrix);

    void Init();
    const std::unordered_map<GizmoType, std::vector<Matrix4x4>>& GetGizmosToDraw();
    std::shared_ptr<DrawableGeometry> GetGizmosGeometry(GizmoType gizmoType);
    void ClearGizmos();

    bool IsEnabled();
    void SetEnabled(bool enabled);
} // namespace Gizmos

#endif

#endif