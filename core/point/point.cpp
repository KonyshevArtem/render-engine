#include "point.h"
#include "graphics_backend_api.h"

Point::Point() :
    DrawableGeometry(PrimitiveType::POINTS, 1, false)
{
    m_VertexAttributes.Add({0, 3, VertexAttributeDataType::FLOAT, false, 0, 0});

    float zero[3] {0, 0, 0};
    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(sizeof(zero), BufferBindTarget::ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->SetBufferData(vertexBuffer, 0, sizeof(zero), &zero);

    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, {}, m_VertexAttributes.GetAttributes());
}