#include "drawable_geometry.h"

DrawableGeometry::DrawableGeometry()
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_VertexBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
}

DrawableGeometry::~DrawableGeometry()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_VertexBuffer);
}