#include "renderer.h"
#include "billboard_renderer.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics.h"
#include "material/material.h"
#include "texture_2d/texture_2d.h"
#include "graphics/render_queue/render_queue.h"
#include "graphics_buffer/graphics_buffer_view.h"

#include <utility>

Renderer::Renderer(const std::shared_ptr<Material>& material) :
    m_Material(material)
{
}

Renderer::~Renderer()
{
    SetMatricesBufferView(nullptr);
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

std::shared_mutex& Renderer::GetMatricesBufferViewMutex()
{
    return m_MatricesBufferViewMutex;
}

bool Renderer::IsTransformDirty() const
{
    return m_TransformDirty;
}

void Renderer::SetTransformDirty(bool dirty)
{
    m_TransformDirty = dirty;
}

void Renderer::SetMatricesBufferView(const std::shared_ptr<GraphicsBufferView>& view)
{
    RenderQueue::FreeMatricesEntry(m_MatricesBufferView);
    m_MatricesBufferView = view;
}

std::shared_ptr<GraphicsBufferView> Renderer::GetMatricesBufferView() const
{
    return m_MatricesBufferView;
}
