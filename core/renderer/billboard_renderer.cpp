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

shared_ptr<Shader> BillboardRenderer::m_Shader = nullptr;

BillboardRenderer::BillboardRenderer(shared_ptr<GameObject> _gameObject, shared_ptr<Texture2D> _texture) :
    Renderer(std::move(_gameObject)), m_Texture(std::move(_texture))
{
    if (m_Shader == nullptr)
        m_Shader = Shader::Load("resources/shaders/billboard/billboard.shader", vector<string>());

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

void BillboardRenderer::Render()
{
    if (m_Shader == nullptr || m_Texture == nullptr || m_GameObject == nullptr || m_Texture->Width == 0 || m_Texture->Height == 0)
        return;

    m_Shader->Use();
    m_Shader->SetTextureUniform("_Texture", m_Texture);

    float aspect = static_cast<float>(m_Texture->Width) / m_Texture->Height;
    Vector2 size {m_Size, m_Size / aspect};
    m_Shader->SetUniform("_Size", &size);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), &m_GameObject->LocalPosition);
    glDrawArrays(GL_POINTS, 0, 1);

    glBindVertexArray(0);
    Shader::DetachCurrentShader();
}

void BillboardRenderer::Render(const shared_ptr<Shader> &_shader)
{
    Render();
}
