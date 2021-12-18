#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "mesh.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"

Mesh::Mesh(vector<Vector3> &_vertices,
           vector<Vector3> &_normals,
           vector<int> &    _indexes,
           vector<Vector2> &_uvs,
           vector<Vector3> &_tangents) :
    m_Vertices(std::move(_vertices)),
    m_Normals(std::move(_normals)),
    m_Indexes(std::move(_indexes)),
    m_UVs(std::move(_uvs)),
    m_Tangents(std::move(_tangents))
{
}

void Mesh::Init()
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    long vertexSize   = sizeof(Vector3) * m_Vertices.size();
    long normalsSize  = sizeof(Vector3) * m_Normals.size();
    long indexSize    = sizeof(int) * m_Indexes.size();
    long uvSize       = sizeof(Vector2) * m_UVs.size();
    long tangentsSize = sizeof(Vector3) * m_Tangents.size();

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

void Mesh::Draw() const
{
    glBindVertexArray(m_VertexArrayObject);
    glDrawElements(GL_TRIANGLES, m_Indexes.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
}
