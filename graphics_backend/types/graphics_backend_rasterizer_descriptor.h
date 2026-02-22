#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_RASTERIZER_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_RASTERIZER_DESCRIPTOR_H

#include "enums/cull_face.h"
#include "enums/cull_face_orientation.h"

struct GraphicsBackendRasterizerDescriptor
{
    CullFace Face = CullFace::BACK;
    CullFaceOrientation Orientation = CullFaceOrientation::CLOCKWISE;

    static GraphicsBackendRasterizerDescriptor NoCull()
    {
        return { CullFace::NONE, CullFaceOrientation::CLOCKWISE };
    }

    static GraphicsBackendRasterizerDescriptor CullFront()
    {
        return { CullFace::FRONT, CullFaceOrientation::CLOCKWISE };
    }
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_RASTERIZER_DESCRIPTOR_H
