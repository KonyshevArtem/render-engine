#ifndef RENDER_ENGINE_RING_BUFFER_H
#define RENDER_ENGINE_RING_BUFFER_H

#include "enums/buffer_usage_hint.h"
#include "types/graphics_backend_resource_bindings.h"

#include <cstdint>
#include <memory>
#include <vector>

class GraphicsBuffer;
class GraphicsBackendBuffer;

class RingBuffer
{
public:
    RingBuffer(uint64_t size, BufferUsageHint usageHint);
    ~RingBuffer() = default;

    const GraphicsBackendBuffer& GetBackendBuffer() const;
    void SetData(const void *data, uint64_t offset, uint64_t size);
    void Reset();

    inline uint64_t GetOffset() const
    {
        return m_Size * (m_CurrentOffset % m_Capacity);
    }

    inline uint64_t GetSize() const
    {
        return m_Size;
    }

    RingBuffer(const RingBuffer &) = delete;
    RingBuffer(RingBuffer &&)      = delete;

    RingBuffer &operator()(const RingBuffer &) = delete;
    RingBuffer &operator()(RingBuffer &&)      = delete;

private:
    std::vector<std::shared_ptr<GraphicsBuffer>> m_Buffers;

    BufferUsageHint m_UsageHint;
    uint64_t m_Size;
    int m_Capacity;

    int m_CurrentOffset = -1;
};


#endif //RENDER_ENGINE_RING_BUFFER_H
