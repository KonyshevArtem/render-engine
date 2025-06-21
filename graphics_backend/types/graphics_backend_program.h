#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H

#include <cstdint>

class GraphicsBackendProgram
{
private:
    uint64_t Program;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
