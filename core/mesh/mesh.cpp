#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "mesh.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"

Mesh::Mesh(const vector<Vector3> &_vertices,
           const vector<Vector3> &_normals,
           const vector<int>     &_indexes,
           const vector<Vector2> &_uvs)
{
    m_Vertices = _vertices;
    m_Normals  = _normals;
    m_Indexes  = _indexes;
    m_UVs      = _uvs;
}

void Mesh::Init()
{
    glGenVertexArrays(1, &m_VertexArrayObject);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);

    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

    long vertexSize  = sizeof(Vector3) * m_Vertices.size();
    long normalsSize = sizeof(Vector3) * m_Normals.size();
    long indexSize   = sizeof(int) * m_Indexes.size();
    long uvSize      = sizeof(Vector2) * m_UVs.size();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (uvSize > 0)
        glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *) vertexSize);
    if (uvSize > 0)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *) (vertexSize + normalsSize));

    glBufferData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, m_Indexes.data(), GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, m_Vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalsSize, m_Normals.data());
    if (uvSize > 0)
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize, uvSize, m_UVs.data());

    glBindVertexArray(0);
}

int Mesh::GetTrianglesCount()
{
    return m_Indexes.size() / 3;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
}
