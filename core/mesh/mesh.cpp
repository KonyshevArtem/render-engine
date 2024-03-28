#include "mesh.h"
#include "vector2/vector2.h"
#include "graphics_backend_api.h"

#include <span>

Mesh::Mesh(const std::vector<Vector3> &_vertices,
           const std::vector<Vector3> &_normals,
           const std::vector<int>     &_indexes,
           const std::vector<Vector2> &_uvs,
           const std::vector<Vector3> &_tangents) :
    DrawableGeometry(PrimitiveType::TRIANGLES, _indexes.size(), true),
    m_Bounds(Bounds::FromPoints(std::span<const Vector3> {_vertices.data(), _vertices.size()}))
{
    int vertexSize   = sizeof(Vector3) * _vertices.size();
    int normalsSize  = sizeof(Vector3) * _normals.size();
    int indexSize    = sizeof(int) * _indexes.size();
    int uvSize       = sizeof(Vector2) * _uvs.size();
    int tangentsSize = sizeof(Vector3) * _tangents.size();

    bool hasNormals  = normalsSize > 0;
    bool hasUV       = uvSize > 0;
    bool hasTangents = tangentsSize > 0;

    auto vertexBuffer = GraphicsBackend::Current()->CreateBuffer(vertexSize + normalsSize + uvSize + tangentsSize, BufferBindTarget::ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    auto indexBuffer = GraphicsBackend::Current()->CreateBuffer(indexSize, BufferBindTarget::ELEMENT_ARRAY_BUFFER, BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->SetBufferData(indexBuffer, 0, indexSize, _indexes.data());

    std::vector<GraphicsBackendVertexAttributeDescriptor> attributes;

    attributes.push_back({0, 3, VertexAttributeDataType::FLOAT, false, 0, 0});
    GraphicsBackend::Current()->SetBufferData(vertexBuffer, 0, vertexSize, _vertices.data());

    if (hasNormals)
    {
        attributes.push_back({1, 3, VertexAttributeDataType::FLOAT, false, 0, vertexSize});
        GraphicsBackend::Current()->SetBufferData(vertexBuffer, vertexSize, normalsSize, _normals.data());
    }
    if (hasUV)
    {
        attributes.push_back({2, 2, VertexAttributeDataType::FLOAT, false, 0, vertexSize + normalsSize});
        GraphicsBackend::Current()->SetBufferData(vertexBuffer, vertexSize + normalsSize, uvSize, _uvs.data());
    }
    if (hasTangents)
    {
        attributes.push_back({3, 3, VertexAttributeDataType::FLOAT, false, 0, vertexSize + normalsSize + uvSize});
        GraphicsBackend::Current()->SetBufferData(vertexBuffer, vertexSize + normalsSize + uvSize, tangentsSize, _tangents.data());
    }

    m_GraphicsBackendGeometry = GraphicsBackend::Current()->CreateGeometry(vertexBuffer, indexBuffer, attributes);
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
        fullscreenMesh = std::make_shared<Mesh>(vertices, normals, indexes, uvs, tangents);
    }


    return fullscreenMesh;
}