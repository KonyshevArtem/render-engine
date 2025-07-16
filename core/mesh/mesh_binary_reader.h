#ifndef RENDER_ENGINE_MESH_BINARY_READER_H
#define RENDER_ENGINE_MESH_BINARY_READER_H

#include "mesh_header.h"

#include <filesystem>
#include <vector>
#include <span>

class MeshBinaryReader
{
public:
    MeshBinaryReader() = default;

    bool ReadMesh(const std::filesystem::path &path);

    const std::span<uint8_t>& GetVertexData() const
    {
        return m_VertexData;
    }

    const std::span<int>& GetIndices() const
    {
        return m_Indices;
    }

    const MeshHeader& GetHeader() const
    {
        return m_Header;
    }

private:
    std::vector<uint8_t> m_MeshBinaryData;
    std::span<uint8_t> m_VertexData;
    std::span<int> m_Indices;
    MeshHeader m_Header{};
};


#endif //RENDER_ENGINE_MESH_BINARY_READER_H
