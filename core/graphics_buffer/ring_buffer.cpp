#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics_backend_api.h"
#include "graphics/graphics.h"

const int k_DefaultCapacity = 64;

uint64_t AlignSize(uint64_t size)
{
    int alignment = GraphicsBackend::Current()->GetConstantBufferOffsetAlignment();
    if (size % alignment != 0)
    {
        size = (size / alignment + 1) * alignment;
    }

    return size;
}

RingBuffer::RingBuffer(uint64_t elementSize, const std::string& name) :
    m_ElementSize(AlignSize(elementSize)),
    m_Capacity(k_DefaultCapacity)
{
    m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, name);
}

void RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    ++m_CurrentOffset;
    if (m_CurrentOffset >= m_Capacity)
    {
        m_Capacity += k_DefaultCapacity;
        m_Buffer->Resize(m_ElementSize * m_Capacity);
    }

    m_Buffer->SetData(data, m_ElementSize * m_CurrentOffset + offset, size);
}

void RingBuffer::Reset()
{
    m_CurrentOffset = -1;
}
