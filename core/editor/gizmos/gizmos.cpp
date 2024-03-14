#if RENDER_ENGINE_EDITOR

#include "gizmos.h"
#include "vector3/vector3.h"
#include "matrix4x4/matrix4x4.h"
#include "lines/lines.h"

bool m_IsEnabled;
std::shared_ptr<Lines> m_WireCubePrimitive;
std::unordered_map<std::shared_ptr<DrawableGeometry>, std::vector<Matrix4x4>> m_GizmosToDraw;

void Gizmos::DrawWireCube(const Matrix4x4 &_matrix)
{
    if (m_IsEnabled)
    {
        m_GizmosToDraw[m_WireCubePrimitive].push_back(_matrix);
    }
}

void Gizmos::Init()
{
    // wire cube
    {
        std::vector<Vector3> wireCubePoints
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
        std::vector<int> wireCubeIndices
        {
            0, 1, 0, 2, 1, 3, 2, 3, 4, 5, 4, 6, 5, 7, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7
        };

        m_WireCubePrimitive = std::make_shared<Lines>(wireCubePoints, wireCubeIndices);
        m_GizmosToDraw[m_WireCubePrimitive] = {};
    }
}

const std::unordered_map<std::shared_ptr<DrawableGeometry>, std::vector<Matrix4x4>> &Gizmos::GetGizmosToDraw()
{
    return m_GizmosToDraw;
}

void Gizmos::ClearGizmos()
{
    for (auto &pair : m_GizmosToDraw)
    {
        pair.second.clear();
    }
}

bool Gizmos::IsEnabled()
{
    return m_IsEnabled;
}

void Gizmos::SetEnabled(bool enabled)
{
    m_IsEnabled = enabled;
}

#endif
