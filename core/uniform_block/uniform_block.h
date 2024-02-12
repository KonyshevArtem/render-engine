#ifndef OPENGL_STUDY_UNIFORM_BLOCK_H
#define OPENGL_STUDY_UNIFORM_BLOCK_H

#include "types/graphics_backend_buffer.h"
#include "enums/buffer_usage_hint.h"

class UniformBlock
{
public:
    UniformBlock(uint64_t size, BufferUsageHint usageHint);
    ~UniformBlock();

    void Bind(int binding) const;
    void SetData(const void *data, uint64_t offset, uint64_t size);

    UniformBlock(const UniformBlock &) = delete;
    UniformBlock(UniformBlock &&)      = delete;

    UniformBlock &operator()(const UniformBlock &) = delete;
    UniformBlock &operator()(UniformBlock &&)      = delete;

private:
    GraphicsBackendBuffer m_Buffer{};
    uint64_t m_Size;
};

#endif //OPENGL_STUDY_UNIFORM_BLOCK_H
