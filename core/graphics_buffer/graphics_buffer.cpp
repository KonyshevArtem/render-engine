#include "graphics_buffer.h"
#include "graphics_backend_api.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(uint64_t size, BufferUsageHint usageHint) :
        m_UsageHint(usageHint),
        m_Size(size)
{
    m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, usageHint);
}

GraphicsBuffer::~GraphicsBuffer()
{
    GraphicsBackend::Current()->DeleteBuffer(m_Buffer);
}

void GraphicsBuffer::Bind(const GraphicsBackendResourceBindings &bindings, int offset, int size) const
{
    GraphicsBackend::Current()->BindBuffer(m_Buffer, bindings, offset, size);
}

void GraphicsBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    offset = std::min(offset, m_Size);
    size = std::min(size, m_Size - offset);

    GraphicsBackend::Current()->SetBufferData(m_Buffer, offset, size, data);
}

void GraphicsBuffer::Resize(uint64_t size)
{
    if (size > 0)
    {
        GraphicsBackend::Current()->DeleteBuffer(m_Buffer);
        m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, m_UsageHint);
        m_Size = size;
    }
}
