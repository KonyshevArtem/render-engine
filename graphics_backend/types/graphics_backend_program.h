#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H

#include "enums/program_type.h"
#include "types/graphics_backend_program_descriptor.h"

#include <cstdint>

class GraphicsBackendProgram
{
private:
    uint64_t Program;
    ProgramType Type;
    ThreadGroupSize ThreadGroupSize;

    uint32_t TextureBindings;
    uint32_t SamplerBindings;
    uint32_t BufferBindings;
    uint32_t ConstantBufferBindings;
    uint32_t RWTextureBindings;
    uint32_t RWBufferBindings;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
