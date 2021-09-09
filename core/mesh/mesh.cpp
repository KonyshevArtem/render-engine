#include "mesh.h"

void Mesh::Init()
{
    glGenVertexArrays(1, &this->m_VertexArrayObject);
    glBindVertexArray(this->m_VertexArrayObject);

    GLuint vertexBuffer;
    GLuint indexBuffer;

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    long vertexSize  = GetVertexDataSize();
    long normalsSize = GetNormalsDataSize();
    long indexSize   = GetIndexDataSize();
    long uvSize      = GetUVDataSize();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (uvSize > 0)
        glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *) vertexSize);
    if (uvSize > 0)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *) (vertexSize + normalsSize));

    glBufferData(GL_ARRAY_BUFFER, vertexSize + normalsSize + uvSize, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, GetIndexData(), GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, GetVertexData());
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, normalsSize, GetNormalsData());
    if (uvSize > 0)
        glBufferSubData(GL_ARRAY_BUFFER, vertexSize + normalsSize, uvSize, GetUVData());

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
}
