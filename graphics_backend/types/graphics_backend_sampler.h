#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_SAMPLER_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_SAMPLER_H

#include "graphics_backend.h"

class GraphicsBackendSampler
{
private:
    GRAPHICS_BACKEND_TYPE_UINT Sampler;

    friend class GraphicsBackend;
};

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_SAMPLER_H
