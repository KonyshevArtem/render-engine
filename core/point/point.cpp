#include "point.h"

Point::Point()
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_PointBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    Vector3 zero {0, 0, 0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3), &zero, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

Point::~Point()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_PointBuffer);
}

void Point::Draw(const Material &_material, const RenderSettings &_settings) const
{
    const auto &shader = _material.GetShader();

    glBindVertexArray(m_VertexArrayObject);
    for (int i = 0; i < shader->PassesCount(); ++i)
    {
        if (!_settings.TagsMatch(*shader, i))
            continue;

        shader->Use(i);

        Shader::SetPropertyBlock(_material.GetPropertyBlock());

        glDrawArrays(GL_POINTS, 0, 1);
    }

    glBindVertexArray(0);
}

void Point::SetPosition(const Vector3 &_position)
{
    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), &_position);
    glBindVertexArray(0);
}