#include "renderer.h"
#include "gameObject/gameObject.h"
#include "material/material.h"

Renderer::Renderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Material> &_material) :
    m_GameObject(_gameObject), m_Material(_material)
{
}

Matrix4x4 Renderer::GetModelMatrix() const
{
    if (m_GameObject.expired())
        return Matrix4x4::Identity();

    auto go = m_GameObject.lock();
    return go->GetLocalToWorldMatrix();
}

std::shared_ptr<Material> Renderer::GetMaterial() const
{
    return m_Material;
}