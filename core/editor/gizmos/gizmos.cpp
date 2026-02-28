#if RENDER_ENGINE_EDITOR

#include "gizmos.h"
#include "lines/lines.h"
#include "shader/shader.h"
#include "material/material.h"
#include "vector2/vector2.h"
#include "developer_console/developer_console.h"

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
    std::shared_ptr<Lines> s_WireRectPrimitive;
    std::vector<RenderQueue::Item> s_3DGizmosToDraw;
    std::vector<RenderQueue::Item> s_2DGizmosToDraw;

    std::shared_ptr<DrawableGeometry> GetGizmosGeometry(Gizmos::GizmoType gizmoType)
    {
        switch (gizmoType)
        {
            case Gizmos::GizmoType::WIRE_CUBE:
            case Gizmos::GizmoType::FRUSTUM:
                return s_WireCubePrimitive;
            case Gizmos::GizmoType::RECT:
                return s_WireRectPrimitive;
        }

        return nullptr;
    }

    std::shared_ptr<Material> GetGizmosMaterial(Gizmos::GizmoType gizmoType)
    {
        static std::shared_ptr<Material> wireCubeMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING"}), "Wire Cube Gizmo");
        static std::shared_ptr<Material> frustumMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING", "_FRUSTUM_GIZMO"}), "Frustum Gizmo");
        static std::shared_ptr<Material> wireRectMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING"}), "Wire Rect Gizmo");

        wireRectMaterial->DepthDescriptor = GraphicsBackendDepthDescriptor::AlwaysPass();

        switch (gizmoType)
        {
            case Gizmos::GizmoType::WIRE_CUBE:
                return wireCubeMaterial;
            case Gizmos::GizmoType::FRUSTUM:
                return frustumMaterial;
            case Gizmos::GizmoType::RECT:
                return wireRectMaterial;
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

    GizmosLocal::s_3DGizmosToDraw.push_back(item);
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

    GizmosLocal::s_3DGizmosToDraw.push_back(item);
}

void Gizmos::DrawRect(const Vector2& min, const Vector2& max)
{
    if (!GizmosLocal::s_IsEnabled)
        return;

    Vector3 min3 = Vector3(min.x, min.y, 0.5f);
    Vector3 max3 = Vector3(max.x, max.y, 0.5f);

    Matrix4x4 matrix = Matrix4x4::Translation(min3) * Matrix4x4::Scale(Vector3(max.x - min.x, max.y - min.y, 1));

    RenderQueue::Item item;
    item.Geometry = GizmosLocal::GetGizmosGeometry(GizmoType::RECT);
    item.Material = GizmosLocal::GetGizmosMaterial(GizmoType::RECT);
    item.Matrix = matrix;
    item.AABB = Bounds{min3, max3};

    GizmosLocal::s_2DGizmosToDraw.push_back(item);
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

    // wire rect
    {
        std::vector<Vector3> wireRectCorners
        {
            {0, 0, 0.5f},
            {1, 0, 0.5f},
            {0, 1, 0.5f},
            {1, 1, 0.5f}
        };

        std::vector<int> wireRectIndices
        {
            0, 1, 1, 3, 3, 2, 0, 2
        };

        GizmosLocal::s_WireRectPrimitive = std::make_shared<Lines>(wireRectCorners, wireRectIndices, "WireRect");
    }

    DeveloperConsole::AddBoolCommand(L"Gizmos.Draw", &GizmosLocal::s_IsEnabled);
}

const std::vector<RenderQueue::Item>& Gizmos::Get3DGizmosToDraw()
{
    return GizmosLocal::s_3DGizmosToDraw;
}

const std::vector<RenderQueue::Item>& Gizmos::Get2DGizmosToDraw()
{
    return GizmosLocal::s_2DGizmosToDraw;
}

void Gizmos::ClearGizmos()
{
    GizmosLocal::s_3DGizmosToDraw.clear();
    GizmosLocal::s_2DGizmosToDraw.clear();
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
