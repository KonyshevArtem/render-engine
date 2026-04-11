#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_RAYTRACING_INSTANCE_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_RAYTRACING_INSTANCE_DESCRIPTOR_H

#include "graphics_backend_blas.h"
#include "graphics_backend_buffer.h"
#include "matrix4x4/matrix4x4.h"

struct GraphicsBackendRaytracingInstanceDescriptor
{
	Matrix4x4 Transform;
	GraphicsBackendBLAS BLAS;
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_RAYTRACING_INSTANCE_DESCRIPTOR_H
