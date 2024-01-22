#ifndef OPENGL_STUDY_BUFFER_USAGE_HINT_H
#define OPENGL_STUDY_BUFFER_USAGE_HINT_H

#include "graphics_backend.h"

enum class BufferUsageHint : GRAPHICS_BACKEND_TYPE_ENUM
{
    STREAM_DRAW = GRAPHICS_BACKEND_STREAM_DRAW,
    STREAM_READ = GRAPHICS_BACKEND_STREAM_READ,
    STREAM_COPY = GRAPHICS_BACKEND_STREAM_COPY,

    STATIC_DRAW = GRAPHICS_BACKEND_STATIC_DRAW,
    STATIC_READ = GRAPHICS_BACKEND_STATIC_READ,
    STATIC_COPY = GRAPHICS_BACKEND_STATIC_COPY,

    DYNAMIC_DRAW = GRAPHICS_BACKEND_DYNAMIC_DRAW,
    DYNAMIC_READ = GRAPHICS_BACKEND_DYNAMIC_READ,
    DYNAMIC_COPY = GRAPHICS_BACKEND_DYNAMIC_COPY,
};

#endif //OPENGL_STUDY_BUFFER_USAGE_HINT_H