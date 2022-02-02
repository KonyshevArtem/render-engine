#include "billboard_renderer.h"
#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"
#include "../shader/shader.h"
#include "../texture_2d/texture_2d.h"
#include <vector>

BillboardRenderer::BillboardRenderer(const std::shared_ptr<GameObject> &_gameObject, std::shared_ptr<Texture2D> _texture) :
    Renderer(_gameObject), m_Texture(std::move(_texture))
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_PointsBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

BillboardRenderer::~BillboardRenderer()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_PointsBuffer);
}

void BillboardRenderer::Render() const
{
    static std::shared_ptr<Shader> shader;

    if (shader == nullptr)
        shader = Shader::Load("resources/shaders/billboard/billboard.shader", {});

    if (shader == nullptr || m_Texture == nullptr)
        return;

    int width  = m_Texture->GetWidth();
    int height = m_Texture->GetHeight();
    if (width == 0 || height == 0)
        return;

    if (!shader->Use())
        return;

    Shader::SetTextureUniform("_Texture", *m_Texture);

    auto    aspect = static_cast<float>(width) / height;
    Vector2 size {m_Size, m_Size / aspect};
    Shader::SetUniform("_Size", &size);

    auto position = GetModelMatrix().GetPosition();

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), &position);
    glDrawArrays(GL_POINTS, 0, 1);

    glBindVertexArray(0);
    Shader::DetachCurrentShader();
}

int BillboardRenderer::GetRenderQueue() const
{
    return m_RenderQueue;
}