#include "uniform_block.h"
#include "graphics_backend_api.h"
#include "enums/uniform_block_parameter.h"
#include "enums/buffer_bind_target.h"

#include <cmath>

UniformBlock::UniformBlock(uint64_t size, BufferUsageHint usageHint)
        : m_Size(size)
{
    GraphicsBackend::GenerateBuffers(1, &m_Buffer);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::SetBufferData(BufferBindTarget::UNIFORM_BUFFER, size, nullptr, usageHint);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);
}

UniformBlock::~UniformBlock()
{
    GraphicsBackend::DeleteBuffers(1, &m_Buffer);
}

void UniformBlock::Bind(int binding) const
{
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::BindBufferRange(BufferBindTarget::UNIFORM_BUFFER, binding, m_Buffer, 0, m_Size);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);
}

void UniformBlock::SetData(const void *data, uint64_t offset, uint64_t size)
{
    offset = std::min(offset, m_Size);
    size = std::min(size, m_Size - offset);

    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, m_Buffer);
    GraphicsBackend::SetBufferSubData(BufferBindTarget::UNIFORM_BUFFER, offset, size, data);
    GraphicsBackend::BindBuffer(BufferBindTarget::UNIFORM_BUFFER, GraphicsBackendBuffer::NONE);
}
