#include "graphics_buffer.h"
#include "graphics_backend_api.h"
#include "enums/uniform_block_parameter.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(BufferBindTarget bindTarget, uint64_t size, BufferUsageHint usageHint) :
        m_BindTarget(bindTarget),
        m_UsageHint(usageHint),
        m_Size(size)
{
    GraphicsBackend::Current()->GenerateBuffers(1, &m_Buffer);

    Bind();
    Resize(size);
}

GraphicsBuffer::~GraphicsBuffer()
{
    GraphicsBackend::Current()->DeleteBuffers(1, &m_Buffer);
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

    Bind();
    GraphicsBackend::Current()->SetBufferSubData(m_BindTarget, offset, size, data);
}

void GraphicsBuffer::Resize(uint64_t size)
{
    if (size > 0)
    {
        Bind();
        GraphicsBackend::Current()->SetBufferData(m_BindTarget, size, nullptr, m_UsageHint);
        m_Size = size;
    }
}
