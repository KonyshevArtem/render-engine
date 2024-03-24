#include "drawable_geometry.h"
#include "graphics_backend_api.h"
#include "enums/buffer_bind_target.h"

DrawableGeometry::DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices)
        : m_PrimitiveType(primitiveType),
          m_ElementsCount(elementsCount),
          m_HasIndices(hasIndices)
{
    GraphicsBackend::Current()->GenerateVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::Current()->GenerateBuffers(1, &m_VertexBuffer);

    GraphicsBackend::Current()->BindVertexArrayObject(m_VertexArrayObject);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ARRAY_BUFFER, m_VertexBuffer);
}

DrawableGeometry::~DrawableGeometry()
{
    GraphicsBackend::Current()->DeleteVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::Current()->DeleteBuffers(1, &m_VertexBuffer);
}