#include "mesh.h"
#include "vector2/vector2.h"
#include "graphics_backend_api.h"

#include <span>

namespace MeshLocal
{
    void FillVertexAttributes(VertexAttributes& attributes, bool hasUV, bool hasNormals, bool hasTangents)
    {
        uint64_t posSize = sizeof(Vector3);
        uint64_t uvSize = hasUV ? sizeof(Vector2) : 0;
        uint64_t normalsSize = hasNormals ? sizeof(Vector3) : 0;
        uint64_t tangentsSize = hasTangents ? sizeof(Vector3): 0;

        uint64_t vertexSize = posSize + uvSize + normalsSize + tangentsSize;

        attributes.Add({VertexAttributeSemantic::POSITION, 3, VertexAttributeDataType::FLOAT, false, vertexSize, 0});
        if (hasNormals)
            attributes.Add({VertexAttributeSemantic::NORMAL, 3, VertexAttributeDataType::FLOAT, false, vertexSize, posSize});
        if (hasUV)
            attributes.Add({VertexAttributeSemantic::TEXCOORD, 2, VertexAttributeDataType::FLOAT, false, vertexSize, posSize + normalsSize});
        if (hasTangents)
            attributes.Add({VertexAttributeSemantic::TANGENT, 3, VertexAttributeDataType::FLOAT, false, vertexSize, posSize + normalsSize + uvSize});
    }

    GraphicsBackendGeometry CreateGeometry(const VertexAttributes& attributes, const uint8_t* vertexData, uint64_t vertexDataSize, const int* indices, uint64_t indicesCount, const std::string& name)
    {
        GraphicsBackendBuffer vertexBuffer = GraphicsBackend::Current()->CreateBuffer(vertexDataSize, name + "_Vertices", false, vertexData);
        GraphicsBackendBuffer indexBuffer = GraphicsBackend::Current()->CreateBuffer(indicesCount * sizeof(int), name + "_Indices", false, indices);

        return GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, attributes.GetAttributes(), name);
    }
}

Mesh::Mesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices, const std::string& name)
    : Mesh(vertices, std::vector<Vector3>(), indices, std::vector<Vector2>(), std::vector<Vector3>(), name)
{
}

Mesh::Mesh(const std::span<uint8_t>& vertexData, const std::span<int>& indices, bool hasUV, bool hasNormals, bool hasTangents,
           const Vector3& minPoint, const Vector3& maxPoint, const std::string& name) :
    DrawableGeometry(PrimitiveType::TRIANGLES, indices.size(), true),
    m_Bounds(minPoint, maxPoint)
{
    MeshLocal::FillVertexAttributes(m_VertexAttributes, hasUV, hasNormals, hasTangents);
    m_GraphicsBackendGeometry = MeshLocal::CreateGeometry(m_VertexAttributes, vertexData.data(), vertexData.size(), indices.data(), indices.size(), name);
}

Mesh::Mesh(const std::vector<Vector3>& vertices,
           const std::vector<Vector3>& normals,
           const std::vector<int>& indexes,
           const std::vector<Vector2>& uvs,
           const std::vector<Vector3>& tangents,
           const std::string& name) :
    DrawableGeometry(PrimitiveType::TRIANGLES, indexes.size(), true),
    m_Bounds(Bounds::FromPoints(std::span<const Vector3> {vertices.data(), vertices.size()}))
{
    size_t vertexCount = vertices.size();
    bool hasUV = !uvs.empty();
    bool hasNormals = !normals.empty();
    bool hasTangents = !tangents.empty();

    uint64_t posSize = sizeof(Vector3);
    uint64_t uvSize = hasUV ? sizeof(Vector2) : 0;
    uint64_t normalsSize = hasNormals ? sizeof(Vector3) : 0;
    uint64_t tangentsSize = hasTangents ? sizeof(Vector3): 0;

    uint64_t vertexSize = posSize + uvSize + normalsSize + tangentsSize;

    MeshLocal::FillVertexAttributes(m_VertexAttributes, hasUV, hasNormals, hasTangents);

    auto vertexData = std::vector<uint8_t>(vertexSize * vertexCount);
    auto vertexDataPtr = vertexData.data();

    for (int i = 0; i < vertexCount; ++i)
    {
        uint64_t currentOffset = i * vertexSize;

        auto v = vertices[i];
        memcpy(vertexDataPtr + currentOffset, &v, posSize);

        if (hasNormals)
        {
            auto n = normals[i];
            memcpy(vertexDataPtr + posSize + currentOffset, &n, normalsSize);
        }
        if (hasUV)
        {
            auto uv = uvs[i];
            memcpy(vertexDataPtr + posSize + normalsSize + currentOffset, &uv, uvSize);
        }
        if (hasTangents)
        {
            auto t = tangents[i];
            memcpy(vertexDataPtr + posSize + normalsSize + uvSize + currentOffset, &t, tangentsSize);
        }
    }

    m_GraphicsBackendGeometry = MeshLocal::CreateGeometry(m_VertexAttributes, vertexData.data(), vertexData.size(), indexes.data(), indexes.size(), name);
}

const std::shared_ptr<Mesh> &Mesh::GetFullscreenMesh()
{
    static std::shared_ptr<Mesh> fullscreenMesh = nullptr;
    if (!fullscreenMesh)
    {
        auto vertices  = std::vector<Vector3> {{-1, -1, 0.5f}, {1, -1, 0.5f}, {1, 1, 0.5f}, {-1, 1, 0.5f}};
        auto normals   = std::vector<Vector3> {};
        auto indexes   = std::vector<int> {0, 2, 1, 0, 3, 2};
        auto uvs       = std::vector<Vector2> {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
        auto tangents  = std::vector<Vector3> {};
        fullscreenMesh = std::make_shared<Mesh>(vertices, normals, indexes, uvs, tangents, "FullscreenMesh");
    }


    return fullscreenMesh;
}