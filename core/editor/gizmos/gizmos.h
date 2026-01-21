#if RENDER_ENGINE_EDITOR

#ifndef RENDER_ENGINE_GIZMOS_H
#define RENDER_ENGINE_GIZMOS_H

#include "graphics/render_queue/render_queue.h"

#include <vector>
#include <memory>

struct Vector2;
struct Matrix4x4;
class DrawableGeometry;

namespace Gizmos
{
    enum class GizmoType
    {
        WIRE_CUBE,
        FRUSTUM,
        RECT,
    };

    void DrawWireCube(const Matrix4x4& matrix);
    void DrawFrustum(const Matrix4x4& matrix);
    void DrawRect(const Vector2& min, const Vector2& max);

    void Init();
    const std::vector<RenderQueue::Item>& Get3DGizmosToDraw();
    const std::vector<RenderQueue::Item>& Get2DGizmosToDraw();
    void ClearGizmos();

    bool IsEnabled();
    void SetEnabled(bool enabled);
} // namespace Gizmos

#endif

#endif