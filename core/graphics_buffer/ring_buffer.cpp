#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics/graphics.h"

uint64_t AlignSize(uint64_t size)
{
    int alignment = GraphicsBackend::Current()->GetConstantBufferOffsetAlignment();
    if (alignment != 0 && size % alignment != 0)
    {
        size = (size / alignment + 1) * alignment;
    }

    return size;
}

RingBuffer::RingBuffer(uint64_t elementSize, uint64_t elementsCount, const std::string& name) :
    m_ElementSize(AlignSize(elementSize)),
    m_Capacity(elementsCount)
{
    m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, name);
}

void RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    m_CurrentOffset = (m_CurrentOffset + 1) % m_Capacity;
    m_Buffer->SetData(data, m_ElementSize * m_CurrentOffset + offset, size);
}
