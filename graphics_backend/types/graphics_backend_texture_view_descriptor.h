#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_VIEW_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_VIEW_DESCRIPTOR_H

#include "enums/texture_internal_format.h"

struct GraphicsBackendTextureViewDescriptor
{
    TextureInternalFormat Format = TextureInternalFormat::RGBA8;
    bool ReadWrite = false;

    bool operator==(const GraphicsBackendTextureViewDescriptor& desc) const = default;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_VIEW_DESCRIPTOR_H
