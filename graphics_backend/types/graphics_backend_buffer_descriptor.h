#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_DESCRIPTOR_H

#include <cstdint>

struct GraphicsBackendBufferDescriptor
{
    uint32_t Size = 0;
    bool AllowCPUWrites = false;
    bool AllowGPUWrites = false;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_BUFFER_DESCRIPTOR_H
