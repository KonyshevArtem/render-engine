#include "point.h"
#include "graphics_backend_api.h"
#include "enums/vertex_attribute_data_type.h"

Point::Point() :
    DrawableGeometry(PrimitiveType::POINTS, 1, false)
{
    GraphicsBackend::Current()->EnableVertexAttributeArray(0);
    GraphicsBackend::Current()->SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);

    float zero[3] {0, 0, 0};
    m_VertexBuffer = GraphicsBackend::Current()->CreateBuffer(sizeof(zero), BufferBindTarget::ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ARRAY_BUFFER, m_VertexBuffer);
    GraphicsBackend::Current()->SetBufferData(m_VertexBuffer, BufferBindTarget::ARRAY_BUFFER, sizeof(zero), &zero, BufferUsageHint::STATIC_DRAW);

    GraphicsBackend::Current()->BindVertexArrayObject(GraphicsBackendVAO::NONE);
}