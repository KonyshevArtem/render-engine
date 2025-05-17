#include "vertex_attributes.h"
#include "hash.h"

size_t GetHash_Internal(const GraphicsBackendVertexAttributeDescriptor &attribute)
{
    size_t hash = 0;
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(attribute.Index));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(attribute.Dimensions));
    hash = Hash::Combine(hash, std::hash<VertexAttributeDataType>{}(attribute.DataType));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(attribute.IsNormalized));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(attribute.Stride));
    hash = Hash::Combine(hash, std::hash<uint8_t>{}(attribute.Offset));
    return hash;
}

void VertexAttributes::Add(const GraphicsBackendVertexAttributeDescriptor &attribute)
{
    m_Attributes.push_back(attribute);
    m_Hash = Hash::Combine(m_Hash, GetHash_Internal(attribute));
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
        hash = Hash::Combine(hash, GetHash_Internal(attribute));
    }

    return hash;
}
