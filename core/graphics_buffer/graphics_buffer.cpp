#include "graphics_buffer.h"
#include "graphics_backend_api.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(uint64_t size, std::string name) :
        m_Size(size),
        m_Name(std::move(name))
{
    m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, m_Name);
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
    if (size > 0 && m_Size != size)
    {
        const GraphicsBackendBuffer oldBuffer = m_Buffer;

        m_Buffer = GraphicsBackend::Current()->CreateBuffer(size, m_Name);
        GraphicsBackend::Current()->BeginCopyPass(m_Name + " Buffer Resize Copy");
        GraphicsBackend::Current()->CopyBufferSubData(oldBuffer, m_Buffer, 0, 0, m_Size);
        GraphicsBackend::Current()->EndCopyPass();

        GraphicsBackend::Current()->DeleteBuffer(oldBuffer);
        m_Size = size;
    }
}
