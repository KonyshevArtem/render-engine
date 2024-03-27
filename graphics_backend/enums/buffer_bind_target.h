#ifndef RENDER_ENGINE_BUFFER_BIND_TARGET_H
#define RENDER_ENGINE_BUFFER_BIND_TARGET_H

enum class BufferBindTarget
{
    ARRAY_BUFFER,
    COPY_READ_BUFFER,
    COPY_WRITE_BUFFER,
    DRAW_INDIRECT_BUFFER,
    ELEMENT_ARRAY_BUFFER,
    PIXEL_PACK_BUFFER,
    PIXEL_UNPACK_BUFFER,
    TEXTURE_BUFFER,
    TRANSFORM_FEEDBACK_BUFFER,
    UNIFORM_BUFFER,
    DISPATCH_INDIRECT_BUFFER,
    QUERY_BUFFER,
    SHADER_STORAGE_BUFFER,
};

#endif //RENDER_ENGINE_BUFFER_BIND_TARGET_H
