#include "graphics_buffer.h"

#include <algorithm>

GraphicsBuffer::GraphicsBuffer(uint64_t size, std::string name, bool doubleBuffered) :
        m_Size(size),
        m_Name(std::move(name)),
        m_DoubleBuffered(doubleBuffered)
{
    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        if (i == 0 || m_DoubleBuffered)
            m_Buffer[i] = GraphicsBackend::Current()->CreateBuffer(size, m_Name + "_0", true);
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

void GraphicsBuffer::Resize(uint64_t size)
{
    if (size > 0 && m_Size != size)
    {
        GraphicsBackend::Current()->BeginCopyPass(m_Name + " Buffer Resize Copy");

        for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
        {
            if (i != 0 && !m_DoubleBuffered)
                break;

            const GraphicsBackendBuffer oldBuffer = m_Buffer[i];
            m_Buffer[i] = GraphicsBackend::Current()->CreateBuffer(size, m_Name, true);
            GraphicsBackend::Current()->CopyBufferSubData(oldBuffer, m_Buffer[i], 0, 0, m_Size);
            GraphicsBackend::Current()->DeleteBuffer(oldBuffer);
        }

        GraphicsBackend::Current()->EndCopyPass();
        m_Size = size;
    }
}
