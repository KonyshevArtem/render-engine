#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_DESCRIPTOR_H

#include "enums/comparison_function.h"

struct GraphicsBackendDepthDescriptor
{
    bool WriteDepth = true;
    ComparisonFunction DepthFunction = ComparisonFunction::LEQUAL;

    static GraphicsBackendDepthDescriptor AlwaysPass()
    {
        return { true, ComparisonFunction::ALWAYS };
    }

    static GraphicsBackendDepthDescriptor AlwaysPassNoWrite()
    {
        return { false, ComparisonFunction::ALWAYS };
    }
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEPTH_DESCRIPTOR_H
