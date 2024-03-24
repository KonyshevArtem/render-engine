#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H

#include "graphics_backend.h"

class GraphicsBackendSampler
{
private:
    GRAPHICS_BACKEND_TYPE_UINT Sampler;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
