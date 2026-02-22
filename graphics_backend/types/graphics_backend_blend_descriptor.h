#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H

#include "enums/blend_factor.h"

struct GraphicsBackendBlendDescriptor
{
    bool Enabled = false;
    BlendFactor SourceFactor = BlendFactor::ONE;
    BlendFactor DestinationFactor = BlendFactor::ONE;

    static GraphicsBackendBlendDescriptor PremultipliedAlphaBlending()
    {
        return { true, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA };
    }

    static GraphicsBackendBlendDescriptor AlphaBlending()
    {
        return { true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA };
    }
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H