#include "point.h"
#include "graphics_backend_api.h"

Point::Point(const std::string& name) :
    DrawableGeometry(PrimitiveType::POINTS, 1, false)
{
    m_VertexAttributes.Add({VertexAttributeSemantic::POSITION, 3, VertexAttributeDataType::FLOAT, false, 0, 0});

    float zero[3] {0, 0, 0};
    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(sizeof(zero), name + "_Vertex", false, &zero);
    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, {}, m_VertexAttributes.GetAttributes(), name);
}