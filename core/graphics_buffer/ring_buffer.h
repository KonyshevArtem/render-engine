#ifndef RENDER_ENGINE_RING_BUFFER_H
#define RENDER_ENGINE_RING_BUFFER_H

#include "graphics_buffer.h"

#include <cstdint>
#include <memory>
#include <string>

class GraphicsBuffer;
class GraphicsBackendBuffer;

class RingBuffer
{
public:
    RingBuffer(uint64_t size, const std::string& name);
    ~RingBuffer() = default;

    const GraphicsBackendBuffer& GetBackendBuffer() const
    {
        return m_Buffer->GetBackendBuffer();
    }

    uint64_t SetData(const void *data, uint64_t offset, uint64_t size);

    RingBuffer(const RingBuffer &) = delete;
    RingBuffer(RingBuffer &&) = delete;

    RingBuffer &operator()(const RingBuffer &) = delete;
    RingBuffer &operator()(RingBuffer &&) = delete;

private:
    std::shared_ptr<GraphicsBuffer> m_Buffer;
    std::string m_Name;

    uint64_t m_CurrentOffset[GraphicsBackend::GetMaxFramesInFlight()];
    uint64_t m_LastCheckFrame;
};


#endif //RENDER_ENGINE_RING_BUFFER_H
