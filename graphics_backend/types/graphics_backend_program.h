#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H

#include <cstdint>

class GraphicsBackendProgram
{
private:
    uint64_t Program;
    uint64_t Reflection;
    uint64_t BlendState;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
