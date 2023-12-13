#include "mesh.h"
#include "debug.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"
#include <span>

Mesh::Mesh(const std::vector<Vector3> &_vertices,
           const std::vector<Vector3> &_normals,
           const std::vector<int>     &_indexes,
           const std::vector<Vector2> &_uvs,
           const std::vector<Vector3> &_tangents) :
    DrawableGeometry(),
    m_Bounds(Bounds::FromPoints(std::span<const Vector3> {_vertices.data(), _vertices.size()})),
    m_IndicesCount(_indexes.size())
{
    CHECK_GL(glGenBuffers(1, &m_IndexBuffer));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer));

    long long vertexSize   = sizeof(Vector3) * _vertices.size();
    long long normalsSize  = sizeof(Vector3) * _normals.size();
    long long indexSize    = sizeof(int) * _indexes.size();
    long long uvSize       = sizeof(Vector2) * _uvs.size();
    long long tangentsSize = sizeof(Vector3) * _tangents.size();

    bool hasNormals  = normalsSize > 0;
    bool hasUV       = uvSize > 0;
    bool hasTangents = tangentsSize > 0;

    CHECK_GL(glEnableVertexAttribArray(0));
    if (hasNormals)
        CHECK_GL(glEnableVertexAttribArray(1));
    if (hasUV)
        CHECK_GL(glEnableVertexAttribArray(2));
    if (hasTangents)
        CHECK_GL(glEnableVertexAttribArray(3));

    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    if (hasNormals)
        CHECK_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>(vertexSize)));
    if (hasUV)
        CHECK_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>((vertexSize + normalsSize))));
    if (hasTangents)
        CHECK_GL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>((vertexSize + normalsSize + uvSize))));

    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize + tangentsSize, nullptr, GL_STATIC_DRAW));
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, _indexes.data(), GL_STATIC_DRAW));

    CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, _vertices.data()));
    if (hasNormals)
        CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalsSize, _normals.data()));
    if (hasUV)
        CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize, uvSize, _uvs.data()));
    if (hasTangents)
        CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize, tangentsSize, _tangents.data()));

    CHECK_GL(glBindVertexArray(0));
}

Mesh::~Mesh()
{
    CHECK_GL(glDeleteBuffers(1, &m_IndexBuffer));
}

Bounds Mesh::GetBounds() const
{
    return m_Bounds;
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