#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics/graphics.h"
#include "math_utils.h"
#include "types/graphics_backend_buffer_descriptor.h"

RingBuffer::RingBuffer(const GraphicsBackendBufferDescriptor& descriptor, const std::string& name) :
    m_Name(name),
    m_LastCheckFrame(0),
	m_Descriptor(descriptor)
{
    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
        m_CurrentOffset[i] = 0;

    m_Buffer = std::make_shared<GraphicsBuffer>(m_Descriptor, m_Name);
}

uint64_t RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    const int frameIndex = GraphicsBackend::GetInFlightFrameIndex();
    const uint64_t currentFrame = GraphicsBackend::Current()->GetFrameNumber();

    if (m_LastCheckFrame != currentFrame)
    {
        m_CurrentOffset[frameIndex] = 0;
        m_LastCheckFrame = currentFrame;
    }

    offset = Math::Align(offset, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment());
    size = Math::Align(size, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment());

    const uint64_t bufferSize = m_Buffer->GetSize();
    const uint64_t currentOffset = m_CurrentOffset[frameIndex];
    const uint64_t requiredSize = currentOffset + offset + size;
    if (m_Buffer->GetSize() < requiredSize)
    {
        m_Descriptor.Size = std::max(bufferSize * 2, requiredSize);

        m_Buffer = std::make_shared<GraphicsBuffer>(m_Descriptor, m_Name);
        m_CurrentOffset[frameIndex] = 0;
    }

    m_Buffer->SetData(data, currentOffset + offset, size);
    m_CurrentOffset[frameIndex] += offset + size;
    return currentOffset;
}
