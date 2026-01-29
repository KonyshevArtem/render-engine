#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_DESCRIPTOR_H

#include "types/graphics_backend_stencil_operation_descriptor.h"
#include <cstdint>

struct GraphicsBackendStencilDescriptor
{
    bool Enabled = false;
    uint8_t ReadMask = 255;
    uint8_t WriteMask = 255;
    GraphicsBackendStencilOperationDescriptor FrontFaceOpDescriptor{};
    GraphicsBackendStencilOperationDescriptor BackFaceOpDescriptor{};
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_DESCRIPTOR_H
