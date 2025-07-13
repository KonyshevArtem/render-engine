#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics/graphics.h"
#include "math_utils.h"

RingBuffer::RingBuffer(uint64_t elementSize, uint64_t elementsCount, const std::string& name) :
    m_Name(name),
    m_ElementSize(Math::Align(elementSize, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment())),
    m_Capacity(elementsCount),
    m_ElementsInUse(0)
{
    m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, m_Name);
}

void RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    m_CurrentOffset = (m_CurrentOffset + 1) % m_Capacity;
    m_Buffer->SetData(data, m_ElementSize * m_CurrentOffset + offset, size);
    ++m_ElementsInUse;
}

void RingBuffer::CheckResize()
{
    if (m_OldBuffer)
        m_OldBuffer = nullptr;

    if (m_ElementsInUse > m_Capacity)
    {
        m_OldBuffer = m_Buffer;
        m_Capacity = m_ElementsInUse * 2;
        m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, m_Name);
    }

    m_ElementsInUse = 0;
}
