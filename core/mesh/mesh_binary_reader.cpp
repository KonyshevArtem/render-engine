#include "mesh_binary_reader.h"
#include "file_system/file_system.h"

bool MeshBinaryReader::ReadMesh(const std::filesystem::path &path)
{
    static constexpr int headerSize = sizeof(MeshHeader);

    m_MeshBinaryData.clear();

    if (!FileSystem::ReadFileBytes(FileSystem::GetResourcesPath() / path, m_MeshBinaryData))
        return false;

    m_Header = *reinterpret_cast<MeshHeader*>(m_MeshBinaryData.data());

    uint8_t* vertexData = reinterpret_cast<uint8_t*>(&m_MeshBinaryData[0] + headerSize);
    m_VertexData = std::span<uint8_t>(vertexData, m_Header.VertexDataSize);

    int* indices = reinterpret_cast<int*>(&m_MeshBinaryData[0] + headerSize + m_Header.VertexDataSize);
    m_Indices = std::span<int>(indices, m_Header.IndicesCount);

    return true;
}
