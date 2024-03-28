#include "lines.h"
#include "graphics_backend_api.h"

Lines::Lines(const std::vector<Vector3> &points, const std::vector<int> &indices) :
        DrawableGeometry(PrimitiveType::LINES, indices.size(), true),
        m_Bounds(Bounds::FromPoints(points))
{
    int pointsSize = sizeof(Vector3) * points.size();
    int indicesSize = sizeof(int) * indices.size();

    std::vector<GraphicsBackendVertexAttributeDescriptor> attributes;
    attributes.push_back({0, 3, VertexAttributeDataType::FLOAT, false, 0, 0});

    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(pointsSize, BufferBindTarget::ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    auto indexBuffer = GraphicsBackend::Current()->CreateBuffer(indicesSize, BufferBindTarget::ELEMENT_ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, attributes);

    GraphicsBackend::Current()->SetBufferData(vertexBuffer, 0, pointsSize, points.data());
    GraphicsBackend::Current()->SetBufferData(indexBuffer, 0, indicesSize, indices.data());
}
