#include "renderer.h"
#include "gameObject/gameObject.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"

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

bool Renderer::Comparer::operator()(Renderer *_r1, Renderer *_r2)
{
    if (_r1 == nullptr || _r1->m_GameObject.expired())
        return false;
    if (_r2 == nullptr || _r2->m_GameObject.expired())
        return true;

    int renderQueue1 = _r1->GetRenderQueue();
    int renderQueue2 = _r2->GetRenderQueue();
    if (renderQueue1 != renderQueue2)
        return renderQueue1 < renderQueue2;

    float distance1 = (_r1->m_GameObject.lock()->GetLocalPosition() - CameraPosition).Length();
    float distance2 = (_r2->m_GameObject.lock()->GetLocalPosition() - CameraPosition).Length();
    return Sorting == Sorting::FRONT_TO_BACK ? distance1 < distance2 : distance1 > distance2;
}

Renderer::Filter Renderer::Filter::Opaque()
{
    return Renderer::Filter {[](Renderer *_r)
                             { return _r != nullptr && _r->GetRenderQueue() < TRANSPARENT_RENDER_QUEUE; }};
}

Renderer::Filter Renderer::Filter::Transparent()
{
    return Renderer::Filter {[](Renderer *_r)
                             { return _r != nullptr && _r->GetRenderQueue() >= TRANSPARENT_RENDER_QUEUE; }};
}

Renderer::Filter Renderer::Filter::All()
{
    return Renderer::Filter {[](Renderer *_r)
                             { return true; }};
}

bool Renderer::Filter::operator()(Renderer *_renderer)
{
    return Delegate != nullptr ? Delegate(_renderer) : false;
}