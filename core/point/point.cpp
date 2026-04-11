#include "point.h"
#include "graphics_backend_api.h"
#include "types/graphics_backend_buffer_descriptor.h"

Point::Point(const std::string& name) :
    DrawableGeometry(PrimitiveType::POINTS, 1, false)
{
    m_VertexAttributes.Add({VertexAttributeSemantic::POSITION, 3, VertexAttributeDataType::FLOAT, false, 0, 0});

    constexpr float zero[3] {0, 0, 0};

    GraphicsBackendBufferDescriptor bufferDescriptor;
    bufferDescriptor.Size = sizeof(zero);

    const GraphicsBackendBuffer vertexBuffer = GraphicsBackend::Current()->CreateBuffer(bufferDescriptor, name + "_Vertex", &zero);
    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, {}, m_VertexAttributes.GetAttributes(), GetIndicesDataType(), name);
}
