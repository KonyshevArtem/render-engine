#include "renderer.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics.h"
#include "graphics_buffer/graphics_buffer.h"
#include "material/material.h"
#include "shader/shader.h"
#include "global_constants.h"
#include "graphics_backend_api.h"
#include "shader/shader_pass/shader_pass.h"
#include "types/graphics_backend_buffer_info.h"

#include <vector>

std::shared_ptr<GraphicsBuffer> s_InstanceDataBuffer;
std::vector<uint8_t> s_InstanceData;
int64_t s_InstanceDataOffset = 0;
bool s_InstanceDataDirty = false;

Renderer::Renderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Material> &_material) :
    m_GameObject(_gameObject),
    m_Material(_material),
    m_InstanceDataOffset(-1)
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
    SetInstanceData(name, &value, sizeof(Vector4));
}

void Renderer::SetFloat(const std::string &name, float value)
{
    SetInstanceData(name, &value, sizeof(float));
}

void Renderer::SetMatrix(const std::string &name, const Matrix4x4 &value)
{
    SetInstanceData(name, &value, sizeof(Matrix4x4));
}

void Renderer::SetInt(const std::string &name, int value)
{
    SetInstanceData(name, &value, sizeof(int));
}

uint32_t Renderer::GetInstanceDataIndex() const
{
    if (m_InstanceDataOffset >= 0 && m_InstanceDataBufferInfo)
    {
        return m_InstanceDataOffset / m_InstanceDataBufferInfo->GetSize();
    }

    return 0;
}

uint32_t Renderer::GetInstanceDataOffset() const
{
    return m_InstanceDataOffset;
}

void Renderer::UploadInstanceDataBuffer()
{
    if (!s_InstanceDataDirty)
        return;

    if (!s_InstanceDataBuffer || s_InstanceDataBuffer->GetSize() < s_InstanceData.size())
    {
        std::shared_ptr<GraphicsBuffer> oldBuffer = s_InstanceDataBuffer;
        s_InstanceDataBuffer = std::make_shared<GraphicsBuffer>(s_InstanceData.size(), "PerInstanceData");

        if (oldBuffer)
        {
            GraphicsBackend::Current()->BeginCopyPass("PerInstanceData Buffer Extension");
            Graphics::CopyBufferData(oldBuffer, s_InstanceDataBuffer, 0, 0, oldBuffer->GetSize());
            GraphicsBackend::Current()->EndCopyPass();
        }
    }

    s_InstanceDataBuffer->SetData(s_InstanceData.data(), 0, s_InstanceData.size());
    s_InstanceDataDirty = false;
}

const std::shared_ptr<GraphicsBuffer>& Renderer::GetInstanceDataBuffer()
{
    return s_InstanceDataBuffer;
}

void Renderer::SetInstanceData(const std::string &name, const void *data, uint64_t size)
{
    if (!m_InstanceDataBufferInfo)
    {
        return;
    }

    auto& variables = m_InstanceDataBufferInfo->GetVariables();
    auto it = variables.find(name);
    if (it != variables.end())
    {
        if (m_InstanceDataOffset < 0)
        {
            m_InstanceDataOffset = s_InstanceDataOffset;
            s_InstanceDataOffset += m_InstanceDataBufferInfo->GetSize();
        }

        if (s_InstanceData.size() < m_InstanceDataOffset + m_InstanceDataBufferInfo->GetSize())
        {
            s_InstanceData.resize(m_InstanceDataOffset + m_InstanceDataBufferInfo->GetSize() * 2);
        }

        uint64_t varOffset = it->second;
        memcpy(s_InstanceData.data() + m_InstanceDataOffset + varOffset, data, size);
        s_InstanceDataDirty = true;
    }
}
