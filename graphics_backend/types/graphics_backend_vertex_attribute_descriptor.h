#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H

#include "enums/vertex_attribute_data_type.h"

struct GraphicsBackendVertexAttributeDescriptor
{
    char Index;
    char Dimensions;
    VertexAttributeDataType DataType;
    char IsNormalized;
    int Stride;
    int Offset;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H
