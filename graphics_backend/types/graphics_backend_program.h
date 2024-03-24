#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H

#include "graphics_backend.h"

class GraphicsBackendProgram
{
private:
    GRAPHICS_BACKEND_TYPE_UINT Program;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_PROGRAM_H
