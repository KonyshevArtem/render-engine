#include "graphics_buffer.h"
#include "math_utils.h"
#include "types/graphics_backend_buffer_descriptor.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(const GraphicsBackendBufferDescriptor& descriptor, std::string name, bool doubleBuffered) :
        m_Size(Math::Align(descriptor.Size, GraphicsBackend::Current()->GetConstantBufferOffsetAlignment())),
        m_Name(std::move(name)),
        m_DoubleBuffered(doubleBuffered)
{
    // add extra alignment to the size to handle non-aligned offset
    GraphicsBackendBufferDescriptor resizedDescriptor = descriptor;
    resizedDescriptor.Size += GraphicsBackend::Current()->GetConstantBufferOffsetAlignment();

    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        if (i == 0 || m_DoubleBuffered)
	        m_Buffer[i] = GraphicsBackend::Current()->CreateBuffer(resizedDescriptor, m_Name + "_0");
    }
}

GraphicsBuffer::~GraphicsBuffer()
{
    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        if (i == 0 || m_DoubleBuffered)
            GraphicsBackend::Current()->DeleteBuffer(m_Buffer[i]);
    }
}

void GraphicsBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    offset = std::min(offset, m_Size);
    size = std::min(size, m_Size - offset);

    GraphicsBackend::Current()->SetBufferData(GetBackendBuffer(), offset, size, data);
}
