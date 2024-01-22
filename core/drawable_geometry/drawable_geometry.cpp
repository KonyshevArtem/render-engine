#include "drawable_geometry.h"
#include "enums/buffer_bind_target.h"

DrawableGeometry::DrawableGeometry()
{
    GraphicsBackend::GenerateVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::GenerateBuffers(1, &m_VertexBuffer);

    GraphicsBackend::BindVertexArrayObject(m_VertexArrayObject);
    GraphicsBackend::BindBuffer(BufferBindTarget::ARRAY_BUFFER, m_VertexBuffer);
}

DrawableGeometry::~DrawableGeometry()
{
    GraphicsBackend::DeleteVertexArrayObjects(1, &m_VertexArrayObject);
    GraphicsBackend::DeleteBuffers(1, &m_VertexBuffer);
}