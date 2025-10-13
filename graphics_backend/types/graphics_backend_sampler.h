#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H

#include <cstdint>

class GraphicsBackendSampler
{
private:
    uint64_t Sampler;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_SAMPLER_H
