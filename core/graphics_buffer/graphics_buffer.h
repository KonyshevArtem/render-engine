#ifndef RENDER_ENGINE_GRAPHICS_BUFFER_H
#define RENDER_ENGINE_GRAPHICS_BUFFER_H

#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_resource_bindings.h"

#include <string>

class GraphicsBuffer
{
public:
    GraphicsBuffer(uint64_t size, std::string name);
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
    uint64_t m_Size;
    std::string m_Name;
};

#endif //RENDER_ENGINE_GRAPHICS_BUFFER_H
