#include "point.h"

Point::Point() :
    DrawableGeometry()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    Vector3 zero {0, 0, 0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3), &zero, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Point::Draw(const Material &_material, const RenderSettings &_settings) const
{
    const auto &shader = _material.GetShader();

    glBindVertexArray(GetVertexArrayObject());
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
    glBindVertexArray(GetVertexArrayObject());
    glBindBuffer(GL_ARRAY_BUFFER, GetVertexBuffer());
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), &_position);
    glBindVertexArray(0);
}