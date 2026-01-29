#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_OPERATION_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_OPERATION_DESCRIPTOR_H

#include "enums/stencil_operation.h"
#include "enums/comparison_function.h"

struct GraphicsBackendStencilOperationDescriptor
{
    StencilOperation FailOp;
    StencilOperation DepthFailOp;
    StencilOperation PassOp;
    ComparisonFunction ComparisonFunction = ComparisonFunction::ALWAYS;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_STENCIL_OPERATION_DESCRIPTOR_H
