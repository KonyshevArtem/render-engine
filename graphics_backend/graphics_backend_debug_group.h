#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H

#include "enums/gpu_queue.h"
#include <string>

struct GraphicsBackendDebugGroup
{
    explicit GraphicsBackendDebugGroup(const std::string& name, GPUQueue queue);
    ~GraphicsBackendDebugGroup();

    GPUQueue Queue;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
