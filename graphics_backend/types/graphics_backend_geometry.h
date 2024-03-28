#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_GEOMETRY_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_GEOMETRY_H

#include "graphics_backend_buffer.h"
#include <cstdint>

class GraphicsBackendGeometry
{
private:
    uint64_t VertexArrayObject;
    GraphicsBackendBuffer VertexBuffer;
    GraphicsBackendBuffer IndexBuffer;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_GEOMETRY_H
