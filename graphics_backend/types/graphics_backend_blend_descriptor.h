#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H

#include "enums/blend_factor.h"
#include "enums/color_write_mask.h"

struct GraphicsBackendBlendDescriptor
{
    bool Enabled = false;
    BlendFactor SourceFactor = BlendFactor::ONE;
    BlendFactor DestinationFactor = BlendFactor::ONE;
    ColorWriteMask ColorWriteMask = ColorWriteMask::ALL;

    static GraphicsBackendBlendDescriptor PremultipliedAlphaBlending()
    {
        return { true, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA, ColorWriteMask::ALL };
    }

    static GraphicsBackendBlendDescriptor AlphaBlending()
    {
        return { true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, ColorWriteMask::ALL };
    }

    static GraphicsBackendBlendDescriptor NoColorWrite()
    {
        return { false, BlendFactor::ONE, BlendFactor::ONE, ColorWriteMask::NONE };
    }
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_BLEND_DESCRIPTOR_H