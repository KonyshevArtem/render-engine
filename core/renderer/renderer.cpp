#include "renderer.h"
#include "gameObject/gameObject.h"
#include "material/material.h"
#include "graphics_buffer/graphics_buffer_wrapper.h"

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

void Renderer::SetVector(const std::string &name, const Vector4 &value)
{
    SetDataToBuffers(name, &value, sizeof(Vector4));
}

void Renderer::SetFloat(const std::string &name, float value)
{
    SetDataToBuffers(name, &value, sizeof(float));
}

void Renderer::SetMatrix(const std::string &name, const Matrix4x4 &value)
{
    SetDataToBuffers(name, &value, sizeof(Matrix4x4));
}

void Renderer::SetDataToBuffers(const std::string &name, const void *data, uint64_t size)
{
    if (!m_Material)
    {
        return;
    }

    const auto &shader = m_Material->GetShader();
    if (!shader)
    {
        return;
    }

    if (!m_PerInstanceDataBufferWrapper)
    {
        m_PerInstanceDataBufferWrapper = std::make_shared<GraphicsBufferWrapper>(shader, "PerInstanceData");
    }

    m_PerInstanceDataBufferWrapper->TrySetVariable("PerInstance[0]." + name, data, size);
}