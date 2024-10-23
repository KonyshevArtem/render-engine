#ifndef RENDER_ENGINE_GRAPHICS_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BUFFER_H

#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_resource_bindings.h"
#include "enums/buffer_usage_hint.h"

class GraphicsBuffer
{
public:
    GraphicsBuffer(uint64_t size, BufferUsageHint usageHint);
    ~GraphicsBuffer();

    void Bind(const GraphicsBackendResourceBindings &binding, int offset, int size) const;
    void SetData(const void *data, uint64_t offset, uint64_t size);
    void Resize(uint64_t size);

    inline uint64_t GetSize() const
    {
        return m_Size;
    }

    inline const GraphicsBackendBuffer& GetBackendBuffer() const
    {
        return m_Buffer;
    }

    GraphicsBuffer(const GraphicsBuffer &) = delete;
    GraphicsBuffer(GraphicsBuffer &&)      = delete;

    GraphicsBuffer &operator()(const GraphicsBuffer &) = delete;
    GraphicsBuffer &operator()(GraphicsBuffer &&)      = delete;

private:
    GraphicsBackendBuffer m_Buffer{};
    BufferUsageHint m_UsageHint;
    uint64_t m_Size;
};

#endif //RENDER_ENGINE_GRAPHICS_BUFFER_H
