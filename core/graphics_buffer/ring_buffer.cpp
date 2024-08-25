#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics_backend_api.h"

const int k_MaxCapacity = 100;

uint64_t AlignSize(uint64_t size)
{
    int alignment = GraphicsBackend::Current()->GetConstantBufferOffsetAlignment();
    if (size % alignment != 0)
    {
        size = (size / alignment + 1) * alignment;
    }

    return size;
}

int GetCapacity(uint64_t size)
{
    uint64_t maxSize = GraphicsBackend::Current()->GetMaxConstantBufferSize();

    int capacity = k_MaxCapacity;

    uint64_t bufferSize = size * capacity;
    if (bufferSize > maxSize)
    {
        capacity = maxSize / size;
    }

    return capacity;
}

void AddBuffer(std::vector<std::shared_ptr<GraphicsBuffer>>& buffers, uint64_t size, BufferBindTarget bindTarget, BufferUsageHint usageHint)
{
    auto buffer = std::make_shared<GraphicsBuffer>(bindTarget, size, usageHint);
    buffers.push_back(buffer);
}

RingBuffer::RingBuffer(BufferBindTarget bindTarget, uint64_t size, BufferUsageHint usageHint) :
    m_BindTarget(bindTarget),
    m_UsageHint(usageHint),
    m_Size(AlignSize(size)),
    m_Capacity(GetCapacity(m_Size))
{
    AddBuffer(m_Buffers, m_Size * m_Capacity, m_BindTarget, m_UsageHint);
}

void RingBuffer::Bind(const GraphicsBackendResourceBindings &binding) const
{
    m_Buffers[m_CurrentOffset / m_Capacity]->Bind(binding, m_Size * m_CurrentOffset, m_Size);
}

void RingBuffer::Bind(const GraphicsBackendResourceBindings &binding, int offset, int size) const
{
    m_Buffers[m_CurrentOffset / m_Capacity]->Bind(binding, m_Size * m_CurrentOffset + offset, size);
}

void RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    ++m_CurrentOffset;
    if (m_CurrentOffset > m_Capacity)
    {
        AddBuffer(m_Buffers, m_Size * m_Capacity, m_BindTarget, m_UsageHint);
    }

    m_Buffers[m_CurrentOffset / m_Capacity]->SetData(data, m_Size * m_CurrentOffset + offset, size);
}

void RingBuffer::Reset()
{
    m_CurrentOffset = -1;
}