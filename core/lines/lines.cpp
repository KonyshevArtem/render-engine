#include "lines.h"
#include "graphics_backend_api.h"

Lines::Lines(const std::vector<Vector3>& points, const std::vector<int>& indices, const std::string& name) :
        DrawableGeometry(PrimitiveType::LINES, indices.size(), true),
        m_Bounds(Bounds::FromPoints(points))
{
    int pointsSize = sizeof(Vector3) * points.size();
    int indicesSize = sizeof(int) * indices.size();

    m_VertexAttributes.Add({VertexAttributeSemantic::POSITION, 3, VertexAttributeDataType::FLOAT, false, sizeof(Vector3), 0});

    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(pointsSize, name + "_Vertices", false, points.data());
    auto indexBuffer = GraphicsBackend::Current()->CreateBuffer(indicesSize, name + "_Indices", false, indices.data());
    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, m_VertexAttributes.GetAttributes(), name);
}
