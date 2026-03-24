#include "ring_buffer.h"
#include "graphics_buffer.h"
#include "graphics/graphics.h"
#include "math_utils.h"
#include "types/graphics_backend_buffer_descriptor.h"

RingBuffer::RingBuffer(const GraphicsBackendBufferDescriptor& descriptor, const std::string& name) :
    m_Name(name),
    m_CurrentOffset(0),
    m_LastCheckFrame(0),
	m_Descriptor(descriptor)
{
    m_Buffer = std::make_shared<GraphicsBuffer>(m_Descriptor, m_Name);
}

uint64_t RingBuffer::SetData(const void *data, uint64_t offset, uint64_t size, bool* outResized)
{
    const uint64_t currentFrame = GraphicsBackend::Current()->GetFrameNumber();

    if (m_LastCheckFrame != currentFrame)
    {
        m_CurrentOffset = 0;
        m_LastCheckFrame = currentFrame;
    }

    offset = Math::Align(offset, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment());
    size = Math::Align(size, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment());

    const uint64_t bufferSize = m_Buffer->GetSize();
    const uint64_t currentOffset = m_CurrentOffset;
    const uint64_t requiredSize = currentOffset + offset + size;

    const bool needsResizing = m_Buffer->GetSize() < requiredSize;
    if (needsResizing)
    {
        m_Descriptor.Size = std::max(bufferSize * 2, requiredSize);

        m_Buffer = std::make_shared<GraphicsBuffer>(m_Descriptor, m_Name);
        m_CurrentOffset = 0;
    }

    if(outResized)
		*outResized = needsResizing;

    m_Buffer->SetData(data, currentOffset + offset, size);
    m_CurrentOffset += offset + size;
    return currentOffset;
}
