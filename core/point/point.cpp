#include "point.h"
#include "graphics_backend_api.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/buffer_bind_target.h"
#include "enums/buffer_usage_hint.h"

Point::Point() :
    DrawableGeometry(PrimitiveType::POINTS, 1, false)
{
    GraphicsBackend::EnableVertexAttributeArray(0);
    GraphicsBackend::SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);

    float zero[3] {0, 0, 0};
    GraphicsBackend::SetBufferData(BufferBindTarget::ARRAY_BUFFER, sizeof(zero), &zero, BufferUsageHint::STATIC_DRAW);

    GraphicsBackend::BindVertexArrayObject(GraphicsBackendVAO::NONE);
}