#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H

#include "enums/texture_internal_format.h"
#include "enums/blend_factor.h"

struct GraphicsBackendColorAttachmentDescriptor
{
    TextureInternalFormat Format;
    BlendFactor SourceFactor;
    BlendFactor DestinationFactor;
    bool BlendingEnabled;
    bool IsLinear;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_COLOR_ATTACHMENT_DESCRIPTOR_H
