#include "mesh.h"

const GLuint &Mesh::GetVertexArrayObject() const
{
    return this->vertexArrayObject;
}

void Mesh::Init()
{
    glGenVertexArrays(1, &this->vertexArrayObject);
    glBindVertexArray(this->vertexArrayObject);

    GLuint vertexBuffer;
    GLuint indexBuffer;

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    long vertexSize  = GetVertexDataSize();
    long colorsSize  = GetColorsDataSize();
    long normalsSize = GetNormalsDataSize();
    long indexSize   = GetIndexDataSize();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void *) vertexSize);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *) (vertexSize + colorsSize));

    glBufferData(GL_ARRAY_BUFFER, vertexSize + colorsSize + normalsSize, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, GetIndexData(), GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, GetVertexData());
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, colorsSize, GetColorsData());
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + colorsSize, normalsSize, GetNormalsData());

    glBindVertexArray(0);
}
