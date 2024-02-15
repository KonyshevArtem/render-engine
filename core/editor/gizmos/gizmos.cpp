#if OPENGL_STUDY_EDITOR

#include "gizmos.h"
#include "vector3/vector3.h"
#include "lines/lines.h"
#include "graphics/draw_call_info.h"
#include "shader/shader.h"

std::shared_ptr<Material> m_GizmosMaterial;
std::shared_ptr<Lines> m_WireCubePrimitive;
std::vector<DrawCallInfo> m_DrawInfos;

void Gizmos::DrawWireCube(const Matrix4x4 &_matrix)
{
    m_DrawInfos.push_back(DrawCallInfo{m_WireCubePrimitive, m_GizmosMaterial, _matrix, m_WireCubePrimitive->GetBounds()});
}

void Gizmos::Init()
{
    std::shared_ptr<Shader> gizmosShader = Shader::Load("resources/shaders/gizmos/gizmos.shader", {});
    m_GizmosMaterial = std::make_shared<Material>(gizmosShader);

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
    }
}

const std::vector<DrawCallInfo> &Gizmos::GetDrawInfos()
{
    return m_DrawInfos;
}

void Gizmos::ClearDrawInfos()
{
    m_DrawInfos.clear();
}

#endif
