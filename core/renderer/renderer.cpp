#include "renderer.h"
#include "billboard_renderer.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics.h"
#include "material/material.h"
#include "texture_2d/texture_2d.h"

#include <utility>

Renderer::Renderer(const std::shared_ptr<Material> &_material) :
    m_Material(_material)
{
}

Matrix4x4 Renderer::GetModelMatrix() const
{
    if (m_GameObject.expired())
        return Matrix4x4::Identity();

    auto go = m_GameObject.lock();
    return go->GetLocalToWorldMatrix();
}

std::shared_ptr<Material> Renderer::GetMaterial()
{
    std::shared_lock lock(m_MaterialMutex);
    return m_Material;
}

void Renderer::SetMaterial(std::shared_ptr<Material> material)
{
    std::unique_lock lock(m_MaterialMutex);
    m_Material = std::move(material);
}
