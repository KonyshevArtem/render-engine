#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_DESCRIPTOR_H

#include "enums/texture_internal_format.h"

struct GraphicsBackendTextureDescriptor
{
    uint32_t Width = 1;
    uint32_t Height = 1;
    uint32_t Depth = 1;
    uint32_t MipLevels = 1;
    TextureInternalFormat Format = TextureInternalFormat::RGBA8;
    bool Linear = false;
    bool RenderTarget = false;
    bool ReadWrite = false;

    bool operator==(const GraphicsBackendTextureDescriptor& desc) const = default;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_TEXTURE_DESCRIPTOR_H
