#include "billboard_renderer.h"
#include "material/material.h"
#include "point/point.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include <vector>

BillboardRenderer::BillboardRenderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Texture2D> &_texture) :
    Renderer(_gameObject, nullptr), m_Point(std::make_shared<Point>())
{
    static std::shared_ptr<Shader> shader = Shader::Load("resources/shaders/billboard/billboard.shader", {});

    m_Material = std::make_shared<Material>(shader);
    m_Material->SetTexture("_Texture", _texture);

    m_Aspect = static_cast<float>(_texture->GetWidth()) / _texture->GetHeight();
}

Bounds BillboardRenderer::GetAABB() const
{
    return GetModelMatrix() * m_Bounds;
}

std::shared_ptr<DrawableGeometry> BillboardRenderer::GetGeometry() const
{
    return m_Point;
}

void BillboardRenderer::SetSize(float _size)
{
    auto position = GetModelMatrix().GetPosition();
    m_Bounds.Min  = position - Vector3 {_size, 0, _size};
    m_Bounds.Max  = position + Vector3 {_size, 2 * _size, _size};

    Vector4 size {_size, _size / m_Aspect, 0, 0};
    m_Material->SetVector("_Size", size);
}

void BillboardRenderer::SetRenderQueue(int _renderQueue)
{
    if (m_Material)
        m_Material->SetRenderQueue(_renderQueue);
}