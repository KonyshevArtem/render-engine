#include "lines.h"
#include "graphics_backend_api.h"
#include "types/graphics_backend_buffer_descriptor.h"

Lines::Lines(const std::vector<Vector3>& points, const std::vector<int>& indices, const std::string& name) :
        DrawableGeometry(PrimitiveType::LINES, indices.size(), true),
        m_Bounds(Bounds::FromPoints(points))
{
    m_VertexAttributes.Add({VertexAttributeSemantic::POSITION, 3, VertexAttributeDataType::FLOAT, false, sizeof(Vector3), 0});

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    
    bufferDescriptor.Size = sizeof(Vector3) * points.size();
    const GraphicsBackendBuffer vertexBuffer = GraphicsBackend::Current()->CreateBuffer(bufferDescriptor, name + "_Vertices", points.data());

    bufferDescriptor.Size = sizeof(int) * indices.size();
    const GraphicsBackendBuffer indexBuffer = GraphicsBackend::Current()->CreateBuffer(bufferDescriptor, name + "_Indices", indices.data());
    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, m_VertexAttributes.GetAttributes(), name);
}
