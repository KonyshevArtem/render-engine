#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "billboard_renderer.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"
#include "../gameObject/gameObject.h"
#include "../shader/shader.h"
#include "../texture_2d/texture_2d.h"
#include <vector>

BillboardRenderer::BillboardRenderer(const shared_ptr<GameObject> &_gameObject, shared_ptr<Texture2D> _texture) :
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
    static shared_ptr<Shader> shader;

    if (shader == nullptr)
        shader = Shader::Load("resources/shaders/billboard/billboard.shader", vector<string>());

    if (shader == nullptr || m_Texture == nullptr || m_GameObject.expired())
        return;

    int width  = m_Texture->GetWidth();
    int height = m_Texture->GetHeight();
    if (width == 0 || height == 0)
        return;

    shader->Use();
    shader->SetTextureUniform("_Texture", *m_Texture);

    auto    aspect = static_cast<float>(width) / height;
    Vector2 size {m_Size, m_Size / aspect};
    shader->SetUniform("_Size", &size);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), &m_GameObject.lock()->LocalPosition);
    glDrawArrays(GL_POINTS, 0, 1);

    glBindVertexArray(0);
    Shader::DetachCurrentShader();
}

void BillboardRenderer::Render(const Shader &_shader) const
{
    Render();
}

int BillboardRenderer::GetRenderQueue() const
{
    return m_RenderQueue;
}