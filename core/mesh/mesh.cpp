#include "mesh.h"
#include "vector2/vector2.h"
#include "graphics_backend_api.h"

#include <span>

Mesh::Mesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices, const std::string& name)
    : Mesh(vertices, std::vector<Vector3>(), indices, std::vector<Vector2>(), std::vector<Vector3>(), name)
{
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

    uint64_t indexSize = sizeof(int) * indexes.size();
    uint64_t vertexSize = posSize + uvSize + normalsSize + tangentsSize;

    m_VertexAttributes.Add({0, 3, VertexAttributeDataType::FLOAT, false, vertexSize, 0});
    if (hasNormals)
    {
        m_VertexAttributes.Add({1, 3, VertexAttributeDataType::FLOAT, false, vertexSize, posSize});
    }
    if (hasUV)
    {
        m_VertexAttributes.Add({2, 2, VertexAttributeDataType::FLOAT, false, vertexSize, posSize + normalsSize});
    }
    if (hasTangents)
    {
        m_VertexAttributes.Add({3, 3, VertexAttributeDataType::FLOAT, false, vertexSize, posSize + normalsSize + uvSize});
    }

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

    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(vertexSize * vertexCount, name + "_Vertices", false, vertexDataPtr);
    auto indexBuffer = GraphicsBackend::Current()->CreateBuffer(indexSize, name + "_Indices", false, indexes.data());

    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, m_VertexAttributes.GetAttributes(), name);
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