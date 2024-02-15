#include "graphics_buffer.h"
#include "graphics_backend_api.h"
#include "enums/uniform_block_parameter.h"

#include <cmath>

GraphicsBuffer::GraphicsBuffer(BufferBindTarget bindTarget, uint64_t size, BufferUsageHint usageHint) :
        m_BindTarget(bindTarget),
        m_Size(size)
{
    GraphicsBackend::GenerateBuffers(1, &m_Buffer);

    Bind();
    GraphicsBackend::SetBufferData(m_BindTarget, size, nullptr, usageHint);
    GraphicsBackend::BindBuffer(m_BindTarget, GraphicsBackendBuffer::NONE);
}

GraphicsBuffer::~GraphicsBuffer()
{
    GraphicsBackend::DeleteBuffers(1, &m_Buffer);
}

void GraphicsBuffer::Bind() const
{
    GraphicsBackend::BindBuffer(m_BindTarget, m_Buffer);
}

void GraphicsBuffer::Bind(int binding) const
{
    Bind();
    GraphicsBackend::BindBufferRange(m_BindTarget, binding, m_Buffer, 0, m_Size);
    GraphicsBackend::BindBuffer(m_BindTarget, GraphicsBackendBuffer::NONE);
}

void GraphicsBuffer::SetData(const void *data, uint64_t offset, uint64_t size)
{
    offset = std::min(offset, m_Size);
    size = std::min(size, m_Size - offset);

    Bind();
    GraphicsBackend::SetBufferSubData(m_BindTarget, offset, size, data);
    GraphicsBackend::BindBuffer(m_BindTarget, GraphicsBackendBuffer::NONE);
}
