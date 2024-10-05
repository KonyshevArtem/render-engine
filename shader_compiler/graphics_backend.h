#ifndef RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H
#define RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H

#include <string>

enum GraphicsBackend : int
{
    GRAPHICS_BACKEND_OPENGL = 0,
    GRAPHICS_BACKEND_METAL = 1,

    GRAPHICS_BACKEND_MAX
};

std::string GetBackendLiteral(GraphicsBackend backend)
{
    switch (backend)
    {
        case GRAPHICS_BACKEND_OPENGL:
            return "opengl";
        case GRAPHICS_BACKEND_METAL:
            return "metal";
        default:
            return "";
    }
}

bool TryGetBackend(const std::string& backendString, GraphicsBackend& outBackend)
{
    for (int i = 0; i < GRAPHICS_BACKEND_MAX; ++i)
    {
        auto backend = static_cast<GraphicsBackend>(i);
        if (backendString == GetBackendLiteral(backend))
        {
            outBackend = backend;
            return true;
        }
    }

    return false;
}

#endif //RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H
