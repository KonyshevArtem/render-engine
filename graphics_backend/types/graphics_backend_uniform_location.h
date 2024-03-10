#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_UNIFORM_LOCATION_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_UNIFORM_LOCATION_H

#include "graphics_backend.h"

class GraphicsBackendUniformLocation
{
private:
    GRAPHICS_BACKEND_TYPE_INT UniformLocation;

    friend class GraphicsBackend;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_UNIFORM_LOCATION_H
