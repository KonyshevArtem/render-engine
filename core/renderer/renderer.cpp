#include "renderer.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics.h"
#include "graphics_buffer/graphics_buffer.h"
#include "material/material.h"
#include "shader/shader.h"
#include "global_constants.h"
#include "shader/shader_pass/shader_pass.h"
#include "types/graphics_backend_buffer_info.h"

std::shared_ptr<GraphicsBuffer> s_InstanceDataBuffer;
int64_t s_InstanceDataBufferOffset = 0;

Renderer::Renderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Material> &_material) :
    m_GameObject(_gameObject),
    m_Material(_material),
    m_InstanceDataBufferOffset(-1)
{
    if (_material)
    {
        std::shared_ptr<Shader> shader = _material->GetShader();
        if (shader)
        {
            const auto& buffers = shader->GetPass(0)->GetBuffers();
            auto it = buffers.find(GlobalConstants::PerInstanceDataBufferName);
            if (it != buffers.end())
            {
                m_InstanceDataBufferInfo = it->second;
            }
        }
    }
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

void Renderer::SetInt(const std::string &name, int value)
{
    SetDataToBuffers(name, &value, sizeof(int));
}

uint32_t Renderer::GetInstanceDataIndex() const
{
    if (m_InstanceDataBufferOffset >= 0 && m_InstanceDataBufferInfo)
    {
        return m_InstanceDataBufferOffset / m_InstanceDataBufferInfo->GetSize();
    }

    return 0;
}

uint32_t Renderer::GetInstanceDataOffset() const
{
    return m_InstanceDataBufferOffset;
}

const std::shared_ptr<GraphicsBuffer>& Renderer::GetInstanceDataBuffer()
{
    return s_InstanceDataBuffer;
}

void Renderer::SetDataToBuffers(const std::string &name, const void *data, uint64_t size)
{
    if (!m_InstanceDataBufferInfo)
    {
        return;
    }

    if (m_InstanceDataBufferOffset < 0)
    {
        m_InstanceDataBufferOffset = s_InstanceDataBufferOffset;
        s_InstanceDataBufferOffset += m_InstanceDataBufferInfo->GetSize();
    }

    if (!s_InstanceDataBuffer || s_InstanceDataBuffer->GetSize() < m_InstanceDataBufferOffset + m_InstanceDataBufferInfo->GetSize())
    {
        std::shared_ptr<GraphicsBuffer> oldBuffer = s_InstanceDataBuffer;
        s_InstanceDataBuffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::SHADER_STORAGE_BUFFER, m_InstanceDataBufferOffset + m_InstanceDataBufferInfo->GetSize() * 2, BufferUsageHint::DYNAMIC_DRAW);

        if (oldBuffer)
        {
            Graphics::CopyBufferData(oldBuffer, s_InstanceDataBuffer, 0, 0, oldBuffer->GetSize());
        }
    }

    auto& variables = m_InstanceDataBufferInfo->GetVariables();
    auto it = variables.find(name);
    if (it != variables.end())
    {
        uint64_t varOffset = it->second;
        s_InstanceDataBuffer->SetData(data, m_InstanceDataBufferOffset + varOffset, size);
    }
}
