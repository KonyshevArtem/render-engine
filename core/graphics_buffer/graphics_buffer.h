#ifndef OPENGL_STUDY_GRAPHICS_BUFFER_H
#define OPENGL_STUDY_GRAPHICS_BUFFER_H

#include "types/graphics_backend_buffer.h"
#include "enums/buffer_usage_hint.h"
#include "enums/buffer_bind_target.h"

class GraphicsBuffer
{
public:
    GraphicsBuffer(BufferBindTarget bindTarget, uint64_t size, BufferUsageHint usageHint);
    ~GraphicsBuffer();

    void Bind() const;
    void Bind(int binding) const;
    void SetData(const void *data, uint64_t offset, uint64_t size);

    GraphicsBuffer(const GraphicsBuffer &) = delete;
    GraphicsBuffer(GraphicsBuffer &&)      = delete;

    GraphicsBuffer &operator()(const GraphicsBuffer &) = delete;
    GraphicsBuffer &operator()(GraphicsBuffer &&)      = delete;

private:
    GraphicsBackendBuffer m_Buffer{};
    BufferBindTarget m_BindTarget;
    uint64_t m_Size;
};

#endif //OPENGL_STUDY_GRAPHICS_BUFFER_H
