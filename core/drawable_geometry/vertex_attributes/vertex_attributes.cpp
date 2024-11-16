#include "vertex_attributes.h"
#include "utils/utils.h"

size_t GetHash_Internal(const GraphicsBackendVertexAttributeDescriptor &attribute)
{
    size_t hash = 0;
    hash = Utils::HashCombine(hash, std::hash<uint8_t>{}(attribute.Index));
    hash = Utils::HashCombine(hash, std::hash<uint8_t>{}(attribute.Dimensions));
    hash = Utils::HashCombine(hash, std::hash<VertexAttributeDataType>{}(attribute.DataType));
    hash = Utils::HashCombine(hash, std::hash<uint8_t>{}(attribute.IsNormalized));
    hash = Utils::HashCombine(hash, std::hash<uint8_t>{}(attribute.Stride));
    hash = Utils::HashCombine(hash, std::hash<uint8_t>{}(attribute.Offset));
    return hash;
}

void VertexAttributes::Add(const GraphicsBackendVertexAttributeDescriptor &attribute)
{
    m_Attributes.push_back(attribute);
    m_Hash = Utils::HashCombine(m_Hash, GetHash_Internal(attribute));
}

const std::vector<GraphicsBackendVertexAttributeDescriptor> &VertexAttributes::GetAttributes() const
{
    return m_Attributes;
}

size_t VertexAttributes::GetHash() const
{
    return m_Hash;
}

size_t VertexAttributes::GetHash(const std::vector<GraphicsBackendVertexAttributeDescriptor> &attributes)
{
    size_t hash = 0;

    for (auto &attribute : attributes)
    {
        hash = Utils::HashCombine(hash, GetHash_Internal(attribute));
    }

    return hash;
}
