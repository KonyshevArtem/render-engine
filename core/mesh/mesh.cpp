#include "mesh.h"
#include "vector2/vector2.h"
#include "graphics_backend_api.h"
#include "enums/buffer_usage_hint.h"
#include "enums/buffer_bind_target.h"
#include "enums/vertex_attribute_data_type.h"

#include <span>

Mesh::Mesh(const std::vector<Vector3> &_vertices,
           const std::vector<Vector3> &_normals,
           const std::vector<int>     &_indexes,
           const std::vector<Vector2> &_uvs,
           const std::vector<Vector3> &_tangents) :
    DrawableGeometry(PrimitiveType::TRIANGLES, _indexes.size(), true),
    m_Bounds(Bounds::FromPoints(std::span<const Vector3> {_vertices.data(), _vertices.size()}))
{
    GraphicsBackend::Current()->GenerateBuffers(1, &m_IndexBuffer);
    GraphicsBackend::Current()->BindBuffer(BufferBindTarget::ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    long vertexSize   = sizeof(Vector3) * _vertices.size();
    long normalsSize  = sizeof(Vector3) * _normals.size();
    long indexSize    = sizeof(int) * _indexes.size();
    long uvSize       = sizeof(Vector2) * _uvs.size();
    long tangentsSize = sizeof(Vector3) * _tangents.size();

    bool hasNormals  = normalsSize > 0;
    bool hasUV       = uvSize > 0;
    bool hasTangents = tangentsSize > 0;

    GraphicsBackend::Current()->EnableVertexAttributeArray(0);
    if (hasNormals)
        GraphicsBackend::Current()->EnableVertexAttributeArray(1);
    if (hasUV)
        GraphicsBackend::Current()->EnableVertexAttributeArray(2);
    if (hasTangents)
        GraphicsBackend::Current()->EnableVertexAttributeArray(3);

    GraphicsBackend::Current()->SetVertexAttributePointer(0, 3, VertexAttributeDataType::FLOAT, false, 0, nullptr);
    if (hasNormals)
        GraphicsBackend::Current()->SetVertexAttributePointer(1, 3, VertexAttributeDataType::FLOAT, false, 0, reinterpret_cast<const void *>(vertexSize));
    if (hasUV)
        GraphicsBackend::Current()->SetVertexAttributePointer(2, 2, VertexAttributeDataType::FLOAT, false, 0, reinterpret_cast<const void *>((vertexSize + normalsSize)));
    if (hasTangents)
        GraphicsBackend::Current()->SetVertexAttributePointer(3, 3, VertexAttributeDataType::FLOAT, false, 0, reinterpret_cast<const void *>((vertexSize + normalsSize + uvSize)));

    GraphicsBackend::Current()->SetBufferData(BufferBindTarget::ARRAY_BUFFER, vertexSize + normalsSize + uvSize + tangentsSize, nullptr, BufferUsageHint::STATIC_DRAW);
    GraphicsBackend::Current()->SetBufferData(BufferBindTarget::ELEMENT_ARRAY_BUFFER, indexSize, _indexes.data(), BufferUsageHint::STATIC_DRAW);

    GraphicsBackend::Current()->SetBufferSubData(BufferBindTarget::ARRAY_BUFFER, 0, vertexSize, _vertices.data());
    if (hasNormals)
        GraphicsBackend::Current()->SetBufferSubData(BufferBindTarget::ARRAY_BUFFER, vertexSize, normalsSize, _normals.data());
    if (hasUV)
        GraphicsBackend::Current()->SetBufferSubData(BufferBindTarget::ARRAY_BUFFER, vertexSize + normalsSize, uvSize, _uvs.data());
    if (hasTangents)
        GraphicsBackend::Current()->SetBufferSubData(BufferBindTarget::ARRAY_BUFFER, vertexSize + normalsSize + uvSize, tangentsSize, _tangents.data());

    GraphicsBackend::Current()->BindVertexArrayObject(GraphicsBackendVAO::NONE);
}

Mesh::~Mesh()
{
    GraphicsBackend::Current()->DeleteBuffers(1, &m_IndexBuffer);
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