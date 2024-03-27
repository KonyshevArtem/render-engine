#include "drawable_geometry.h"
#include "graphics_backend_api.h"
#include "enums/buffer_bind_target.h"

DrawableGeometry::DrawableGeometry(PrimitiveType primitiveType, int elementsCount, bool hasIndices)
        : m_PrimitiveType(primitiveType),
          m_ElementsCount(elementsCount),
          m_HasIndices(hasIndices)
{
    GraphicsBackend::Current()->GenerateVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::Current()->BindVertexArrayObject(m_VertexArrayObject);
}

DrawableGeometry::~DrawableGeometry()
{
    GraphicsBackend::Current()->DeleteVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::Current()->DeleteBuffer(m_VertexBuffer);
}