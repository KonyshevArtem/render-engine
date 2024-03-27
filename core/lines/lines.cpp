#include "lines.h"
#include "graphics_backend_api.h"
#include "enums/vertex_attribute_data_type.h"

Lines::Lines(const std::vector<Vector3> &points, const std::vector<int> &indices) :
        DrawableGeometry(PrimitiveType::LINES, indices.size(), true),
        m_Bounds(Bounds::FromPoints(points))
{
    int pointsSize = sizeof(Vector3) * points.size();
    int indicesSize = sizeof(int) * indices.size();

    m_VertexBuffer = GraphicsBackend::Current()->CreateBuffer(pointsSize, BufferBindTarget::ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    m_IndexBuffer = GraphicsBackend::Current()->CreateBuffer(indicesSize, BufferBindTarget::ELEMENT_ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ARRAY_BUFFER, m_VertexBuffer);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    GraphicsBackend::Current()->EnableVertexAttributeArray(0);
    GraphicsBackend::Current()->SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);

    GraphicsBackend::Current()->SetBufferData(m_VertexBuffer, BufferBindTarget::ARRAY_BUFFER, pointsSize, points.data(), BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->SetBufferData(m_IndexBuffer, BufferBindTarget::ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), BufferUsageHint::STATIC_DRAW);

    GraphicsBackend::Current()->BindVertexArrayObject(GraphicsBackendVAO::NONE);
}

Lines::~Lines()
{
    GraphicsBackend::Current()->DeleteBuffer(m_IndexBuffer);
}
