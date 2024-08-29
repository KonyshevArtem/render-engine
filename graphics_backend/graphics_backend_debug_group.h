#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H

#include <string>

struct GraphicsBackendDebugGroup
{
    explicit GraphicsBackendDebugGroup(const std::string &_name);
    ~GraphicsBackendDebugGroup();
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
