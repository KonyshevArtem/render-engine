#ifndef RENDER_ENGINE_RING_BUFFER_H
#define RENDER_ENGINE_RING_BUFFER_H

#include "graphics_buffer.h"
#include "enums/buffer_usage_hint.h"

#include <cstdint>
#include <memory>
#include <string>

class GraphicsBuffer;
class GraphicsBackendBuffer;

class RingBuffer
{
public:
    RingBuffer(uint64_t elementSize, BufferUsageHint usageHint, std::string name);
    ~RingBuffer() = default;

    const GraphicsBackendBuffer& GetBackendBuffer() const
    {
        return m_Buffer->GetBackendBuffer();
    }

    void SetData(const void *data, uint64_t offset, uint64_t size);
    void Reset();

    inline uint64_t GetCurrentElementOffset() const
    {
        return m_ElementSize * m_CurrentOffset;
    }

    inline uint64_t GetElementSize() const
    {
        return m_ElementSize;
    }

    RingBuffer(const RingBuffer &) = delete;
    RingBuffer(RingBuffer &&)      = delete;

    RingBuffer &operator()(const RingBuffer &) = delete;
    RingBuffer &operator()(RingBuffer &&)      = delete;

private:
    std::shared_ptr<GraphicsBuffer> m_Buffer;

    BufferUsageHint m_UsageHint;
    uint64_t m_ElementSize;
    int m_Capacity;
    std::string m_Name;

    int m_CurrentOffset = -1;
};


#endif //RENDER_ENGINE_RING_BUFFER_H
