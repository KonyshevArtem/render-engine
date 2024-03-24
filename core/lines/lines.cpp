#include "lines.h"
#include "graphics_backend_api.h"
#include "enums/buffer_bind_target.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/buffer_usage_hint.h"

Lines::Lines(const std::vector<Vector3> &points, const std::vector<int> &indices) :
        DrawableGeometry(PrimitiveType::LINES, indices.size(), true),
        m_Bounds(Bounds::FromPoints(points))
{
    GraphicsBackend::Current()->GenerateBuffers(1, &m_IndexBuffer);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    long pointsSize = sizeof(Vector3) * points.size();
    long indicesSize = sizeof(int) * indices.size();

    GraphicsBackend::Current()->EnableVertexAttributeArray(0);
    GraphicsBackend::Current()->SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);

    GraphicsBackend::Current()->SetBufferData(BufferBindTarget::ARRAY_BUFFER, pointsSize, points.data(), BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->SetBufferData(BufferBindTarget::ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), BufferUsageHint::STATIC_DRAW);

    GraphicsBackend::Current()->BindVertexArrayObject(GraphicsBackendVAO::NONE);
}

Lines::~Lines()
{
    GraphicsBackend::Current()->DeleteBuffers(1, &m_IndexBuffer);
}
