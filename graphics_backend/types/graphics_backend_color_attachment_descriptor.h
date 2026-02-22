#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H

#include "graphics_backend_blend_descriptor.h"
#include "enums/texture_internal_format.h"

struct GraphicsBackendColorAttachmentDescriptor
{
    TextureInternalFormat Format;
    GraphicsBackendBlendDescriptor BlendDescriptor;
    bool IsLinear;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H
