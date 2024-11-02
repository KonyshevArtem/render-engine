#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H

#include <cstdint>

class GraphicsBackendSampler
{
private:
    uint64_t Sampler;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
