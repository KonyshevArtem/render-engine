#if OPENGL_STUDY_EDITOR

#include "gizmos.h"
#include "vector3/vector3.h"
#include "graphics_backend_api.h"
#include "enums/indices_data_type.h"
#include "enums/primitive_type.h"
#include "enums/buffer_bind_target.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/buffer_usage_hint.h"

std::shared_ptr<Gizmos::GizmosPrimitive> m_WireCubePrimitive;
std::vector<Gizmos::GizmosDrawInfo>      m_DrawInfos;

Gizmos::GizmosPrimitive::GizmosPrimitive(GraphicsBackendVAO _vertexArrayObject, GraphicsBackendBuffer _pointsBuffer, GraphicsBackendBuffer _indexBuffer, int _indexCount) :
        m_VertexArrayObject(_vertexArrayObject), m_PointsBuffer(_pointsBuffer), m_IndexBuffer(_indexBuffer), m_IndexCount(_indexCount)
{
}

Gizmos::GizmosPrimitive::~GizmosPrimitive()
{
    GraphicsBackend::DeleteVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::DeleteBuffers(1, &m_PointsBuffer);
    GraphicsBackend::DeleteBuffers(1, &m_IndexBuffer);
}

void Gizmos::GizmosPrimitive::Draw() const
{
    GraphicsBackend::BindVertexArrayObject(m_VertexArrayObject);
    GraphicsBackend::DrawElements(PrimitiveType::LINES, m_IndexCount, IndicesDataType::UNSIGNED_INT, nullptr);
    GraphicsBackend::BindVertexArrayObject(0);
}

void Gizmos::DrawWireCube(const Matrix4x4 &_matrix)
{
    m_DrawInfos.push_back(Gizmos::GizmosDrawInfo {m_WireCubePrimitive, _matrix});
}

void Gizmos::Init()
{
    constexpr int PRIMITIVES_COUNT = 1;
    GraphicsBackendVAO vertexArrayObjects[PRIMITIVES_COUNT]{0};
    GraphicsBackendBuffer pointsBuffers[PRIMITIVES_COUNT]{0};
    GraphicsBackendBuffer indexBuffers[PRIMITIVES_COUNT]{0};

    GraphicsBackend::GenerateVertexArrayObjects(PRIMITIVES_COUNT, &vertexArrayObjects[0]);
    GraphicsBackend::GenerateBuffers(PRIMITIVES_COUNT, &pointsBuffers[0]);
    GraphicsBackend::GenerateBuffers(PRIMITIVES_COUNT, &indexBuffers[0]);

    // wire cube
    {
        constexpr int INDEX = 0;
        GraphicsBackend::BindVertexArrayObject(vertexArrayObjects[INDEX]);
        GraphicsBackend::BindBuffer(BufferBindTarget::ARRAY_BUFFER, pointsBuffers[INDEX]);
        GraphicsBackend::BindBuffer(BufferBindTarget::ELEMENT_ARRAY_BUFFER, indexBuffers[INDEX]);

        GraphicsBackend::EnableVertexAttributeArray(0);
        GraphicsBackend::SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);

        constexpr int WIRE_CUBE_POINTS_COUNT = 8;
        constexpr int WIRE_CUBE_INDEX_COUNT  = 24;

        Vector3 wireCubePoints[WIRE_CUBE_POINTS_COUNT] {
                {-1, -1, -1},
                {-1, -1, 1},
                {-1, 1, -1},
                {-1, 1, 1},
                {1, -1, -1},
                {1, -1, 1},
                {1, 1, -1},
                {1, 1, 1}};
        int wireCubeIndexes[WIRE_CUBE_INDEX_COUNT] {0, 1, 0, 2, 1, 3, 2, 3, 4, 5, 4, 6, 5, 7, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

        GraphicsBackend::SetBufferData(BufferBindTarget::ARRAY_BUFFER, sizeof(Vector3) * WIRE_CUBE_POINTS_COUNT, &wireCubePoints[0], BufferUsageHint::STATIC_DRAW);
        GraphicsBackend::SetBufferData(BufferBindTarget::ELEMENT_ARRAY_BUFFER, sizeof(int) * WIRE_CUBE_INDEX_COUNT, &wireCubeIndexes[0], BufferUsageHint::STATIC_DRAW);
        GraphicsBackend::BindVertexArrayObject(0);

        m_WireCubePrimitive = std::make_shared<GizmosPrimitive>(vertexArrayObjects[INDEX], pointsBuffers[INDEX], indexBuffers[INDEX], WIRE_CUBE_INDEX_COUNT);
    }
}

std::vector<Gizmos::GizmosDrawInfo> Gizmos::GetDrawInfos()
{
    return m_DrawInfos;
}

void Gizmos::ClearDrawInfos()
{
    m_DrawInfos.clear();
}

#endif
