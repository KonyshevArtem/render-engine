#ifndef RENDER_ENGINE_VERTEX_ATTRIBUTES_H
#define RENDER_ENGINE_VERTEX_ATTRIBUTES_H

#include "types/graphics_backend_vertex_attribute_descriptor.h"

#include <vector>

class VertexAttributes
{
public:
    void Add(const GraphicsBackendVertexAttributeDescriptor &attribute);

    const std::vector<GraphicsBackendVertexAttributeDescriptor> &GetAttributes() const;
    size_t GetHash() const;

    static size_t GetHash(const std::vector<GraphicsBackendVertexAttributeDescriptor> &attributes);

private:
    std::vector<GraphicsBackendVertexAttributeDescriptor> m_Attributes;
    size_t m_Hash = 0;
};

#endif //RENDER_ENGINE_VERTEX_ATTRIBUTES_H
