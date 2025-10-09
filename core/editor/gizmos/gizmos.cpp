#if RENDER_ENGINE_EDITOR

#include "gizmos.h"
#include "lines/lines.h"
#include "shader/shader.h"
#include "material/material.h"

namespace GizmosLocal
{
    std::vector<Vector3> s_CubeCorners
    {
            { -1, -1, -1 },
            { -1, -1, 1 },
            { -1, 1, -1 },
            { -1, 1, 1 },
            { 1, -1, -1 },
            { 1, -1, 1 },
            { 1, 1, -1 },
            { 1, 1, 1 }
    };

    bool s_IsEnabled;
    std::shared_ptr<Lines> s_WireCubePrimitive;
    std::vector<RenderQueue::Item> s_GizmosToDraw;

    std::shared_ptr<DrawableGeometry> GetGizmosGeometry(Gizmos::GizmoType gizmoType)
    {
        switch (gizmoType)
        {
            case Gizmos::GizmoType::WIRE_CUBE:
            case Gizmos::GizmoType::FRUSTUM:
                return s_WireCubePrimitive;
        }

        return nullptr;
    }

    std::shared_ptr<Material> GetGizmosMaterial(Gizmos::GizmoType gizmoType)
    {
        static std::shared_ptr<Material> wireCubeMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING"}, {}, {}, {}), "Wire Cube Gizmo");
        static std::shared_ptr<Material> frustumMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING", "_FRUSTUM_GIZMO"}, {}, {}, {}), "Frustum Gizmo");

        switch (gizmoType)
        {
            case Gizmos::GizmoType::WIRE_CUBE:
                return wireCubeMaterial;
            case Gizmos::GizmoType::FRUSTUM:
                return frustumMaterial;
        }

        return nullptr;
    }
}

void Gizmos::DrawWireCube(const Matrix4x4& matrix)
{
    if (!GizmosLocal::s_IsEnabled)
        return;

    Vector3 points[8];
    for (int i = 0; i < 8; ++i)
        points[i] = matrix * GizmosLocal::s_CubeCorners[i].ToVector4(1);

    RenderQueue::Item item;
    item.Geometry = GizmosLocal::GetGizmosGeometry(GizmoType::WIRE_CUBE);
    item.Material = GizmosLocal::GetGizmosMaterial(GizmoType::WIRE_CUBE);
    item.Matrix = matrix;
    item.AABB = Bounds::FromPoints(std::span<Vector3>(&points[0], 8));

    GizmosLocal::s_GizmosToDraw.push_back(item);
}

void Gizmos::DrawFrustum(const Matrix4x4& matrix)
{
    if (!GizmosLocal::s_IsEnabled)
        return;

    Vector3 points[8];
    for (int i = 0; i < 8; ++i)
    {
        Vector4 point = matrix * GizmosLocal::s_CubeCorners[i].ToVector4(1);
        points[i] = point / point.w;
    }

    RenderQueue::Item item;
    item.Geometry = GizmosLocal::GetGizmosGeometry(GizmoType::FRUSTUM);
    item.Material = GizmosLocal::GetGizmosMaterial(GizmoType::FRUSTUM);
    item.Matrix = matrix;
    item.AABB = Bounds::FromPoints(std::span<Vector3>(&points[0], 8));

    GizmosLocal::s_GizmosToDraw.push_back(item);
}

void Gizmos::Init()
{
    // wire cube
    {
        std::vector<int> wireCubeIndices
        {
            0, 1, 0, 2, 1, 3, 2, 3, 4, 5, 4, 6, 5, 7, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7
        };

        GizmosLocal::s_WireCubePrimitive = std::make_shared<Lines>(GizmosLocal::s_CubeCorners, wireCubeIndices, "WireCube");
    }
}

const std::vector<RenderQueue::Item>& Gizmos::GetGizmosToDraw()
{
    return GizmosLocal::s_GizmosToDraw;
}

void Gizmos::ClearGizmos()
{
    GizmosLocal::s_GizmosToDraw.clear();
}

bool Gizmos::IsEnabled()
{
    return GizmosLocal::s_IsEnabled;
}

void Gizmos::SetEnabled(bool enabled)
{
    GizmosLocal::s_IsEnabled = enabled;
}

#endif
