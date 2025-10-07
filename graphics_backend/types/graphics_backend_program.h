#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H

#include <cstdint>

class GraphicsBackendProgram
{
private:
    uint64_t Program;

    uint32_t TextureBindings;
    uint32_t SamplerBindings;
    uint32_t BufferBindings;
    uint32_t ConstantBufferBindings;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
