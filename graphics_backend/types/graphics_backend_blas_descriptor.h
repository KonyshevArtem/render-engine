#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_DESCRIPTOR_H

#include "types/graphics_backend_geometry.h"

struct GraphicsBackendBLASDescriptor
{
	GraphicsBackendGeometry Geometry;
	int VertexCount;
	int IndexCount;

	GraphicsBackendVertexAttributeDescriptor VertexAttributeDescriptor;
	IndicesDataType IndicesDataType;
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_DESCRIPTOR_H
