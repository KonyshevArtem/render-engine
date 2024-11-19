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

RingBuffer::RingBuffer(uint64_t elementSize, std::string name) :
    m_ElementSize(AlignSize(elementSize)),
    m_Capacity(k_DefaultCapacity),
    m_Name(std::move(name))
{
    m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, m_Name);
}

void RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    ++m_CurrentOffset;
    if (m_CurrentOffset >= m_Capacity)
    {
        m_Capacity += k_DefaultCapacity;

        std::shared_ptr<GraphicsBuffer> oldBuffer = m_Buffer;
        m_Buffer = std::make_shared<GraphicsBuffer>(m_ElementSize * m_Capacity, m_Name);

        if (oldBuffer)
        {
            GraphicsBackend::Current()->BeginCopyPass(m_Name + " Ring Buffer Extension");
            Graphics::CopyBufferData(oldBuffer, m_Buffer, 0, 0, oldBuffer->GetSize());
            GraphicsBackend::Current()->EndCopyPass();
        }
    }

    m_Buffer->SetData(data, m_ElementSize * m_CurrentOffset + offset, size);
}

void RingBuffer::Reset()
{
    m_CurrentOffset = -1;
}
