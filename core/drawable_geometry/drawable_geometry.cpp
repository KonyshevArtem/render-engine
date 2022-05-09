#include "drawable_geometry.h"
#include "core_debug/debug.h"

DrawableGeometry::DrawableGeometry()
{
    CHECK_GL(glGenVertexArrays(1, &m_VertexArrayObject));
    CHECK_GL(glGenBuffers(1, &m_VertexBuffer));

    CHECK_GL(glBindVertexArray(m_VertexArrayObject));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer));
}

DrawableGeometry::~DrawableGeometry()
{
    CHECK_GL(glDeleteVertexArrays(1, &m_VertexArrayObject));
    CHECK_GL(glDeleteBuffers(1, &m_VertexBuffer));
}