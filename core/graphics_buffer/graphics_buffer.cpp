#include "graphics_buffer.h"
#include "graphics_backend_api.h"
#include "enums/uniform_block_parameter.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(BufferBindTarget bindTarget, uint64_t size, BufferUsageHint usageHint) :
        m_BindTarget(bindTarget),
        m_UsageHint(usageHint),
        m_Size(size)
{
    m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, bindTarget, usageHint);
}

GraphicsBuffer::~GraphicsBuffer()
{
    GraphicsBackend::Current()->DeleteBuffer(m_Buffer);
}

void GraphicsBuffer::Bind() const
{
    Bind(m_BindTarget);
}

void GraphicsBuffer::Bind(int binding) const
{
    Bind();
    GraphicsBackend::Current()->BindBufferRange(m_BindTarget, binding, m_Buffer, 0, m_Size);
}

void GraphicsBuffer::Bind(BufferBindTarget bindTarget) const
{
    GraphicsBackend::Current()->BindBuffer(bindTarget, m_Buffer);
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
        m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, m_BindTarget, m_UsageHint);
        m_Size = size;
    }
}
