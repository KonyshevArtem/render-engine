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

    GetVertexes(vertexBuffer);
    GetColors(vertexBuffer);
    GetNormals(vertexBuffer);
    GetIndexes(indexBuffer);

    glBindVertexArray(0);
}
