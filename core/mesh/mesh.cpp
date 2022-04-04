#include "mesh.h"
#include "material/material.h"
#include "shader/shader.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"

Mesh::Mesh(std::vector<Vector3> &_vertices,
           std::vector<Vector3> &_normals,
           std::vector<int>     &_indexes,
           std::vector<Vector2> &_uvs,
           std::vector<Vector3> &_tangents) :
    m_Vertices(std::move(_vertices)),
    m_Normals(std::move(_normals)),
    m_Indexes(std::move(_indexes)),
    m_UVs(std::move(_uvs)),
    m_Tangents(std::move(_tangents)),
    m_Bounds(Bounds::FromPoints(m_Vertices))
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    long long vertexSize   = sizeof(Vector3) * m_Vertices.size();
    long long normalsSize  = sizeof(Vector3) * m_Normals.size();
    long long indexSize    = sizeof(int) * m_Indexes.size();
    long long uvSize       = sizeof(Vector2) * m_UVs.size();
    long long tangentsSize = sizeof(Vector3) * m_Tangents.size();

    bool hasNormals  = normalsSize > 0;
    bool hasUV       = uvSize > 0;
    bool hasTangents = tangentsSize > 0;

    glEnableVertexAttribArray(0);
    if (hasNormals)
        glEnableVertexAttribArray(1);
    if (hasUV)
        glEnableVertexAttribArray(2);
    if (hasTangents)
        glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    if (hasNormals)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>(vertexSize));
    if (hasUV)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>((vertexSize + normalsSize)));
    if (hasTangents)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void *>((vertexSize + normalsSize + uvSize)));

    glBufferData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize + tangentsSize, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, m_Indexes.data(), GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, m_Vertices.data());
    if (hasNormals)
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalsSize, m_Normals.data());
    if (hasUV)
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize, uvSize, m_UVs.data());
    if (hasTangents)
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize, tangentsSize, m_Tangents.data());

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
}

void Mesh::Draw(const Material &_material, const RenderSettings &_settings) const
{
    const auto &shader = _material.GetShader();
    for (int i = 0; i < shader->PassesCount(); ++i)
    {
        if (!_settings.TagsMatch(*shader, i))
            continue;

        shader->Use(i);

        Shader::SetPropertyBlock(_material.GetPropertyBlock());

        glBindVertexArray(m_VertexArrayObject);
        glDrawElements(GL_TRIANGLES, m_Indexes.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
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