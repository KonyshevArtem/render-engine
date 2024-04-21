#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H

#include "enums/vertex_attribute_data_type.h"

struct GraphicsBackendVertexAttributeDescriptor
{
    uint8_t Index;
    uint8_t Dimensions;
    VertexAttributeDataType DataType;
    uint8_t IsNormalized;
    uint64_t Stride;
    uint64_t Offset;
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_VERTEX_ATTRIBUTE_DESCRIPTOR_H
