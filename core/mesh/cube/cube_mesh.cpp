#define GL_SILENCE_DEPRECATION

#include "cube_mesh.h"


static const int   vertexCount    = 24;
static const int   trianglesCount = 12;
static const GLint vertexSize     = sizeof(float) * vertexCount * 3;
static const GLint colorsSize     = sizeof(float) * vertexCount * 4;
static const GLint normalsSize    = sizeof(float) * vertexCount * 3;
static const GLint indexesSize    = sizeof(int) * trianglesCount * 3;

// clang-format off
static const float vertexData[] = {
        // front
        -1, 1, 1,     // 0
        1, 1, 1,      // 1
        1, -1, 1,     // 2
        -1, -1, 1,    // 3

        // back
        -1, 1, -1,      // 4
        1, 1, -1,       // 5
        1, -1, -1,      // 6
        -1, -1, -1,     // 7

        // left
        -1, 1, 1,      // 8
        -1, 1, -1,     // 9
        -1, -1, -1,    // 10
        -1, -1, 1,     // 11

        // right
        1, 1, 1,      // 12
        1, 1, -1,     // 13
        1, -1, -1,    // 14
        1, -1, 1,     // 15

        // top
        1, 1, 1,      // 16
        1, 1, -1,     // 17
        -1, 1, -1,    // 18
        -1, 1, 1,     // 19

        // bottom
        1, -1, 1,      // 20
        1, -1, -1,     // 21
        -1, -1, -1,    // 22
        -1, -1, 1,     // 23
};

static const float colorsData[] = {
        // front
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,

        // back
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,
        0, 1, 0, 1,

        // left
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,

        // right
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,

        // top
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,

        // bottom
        0, 1, 1, 1,
        0, 1, 1, 1,
        0, 1, 1, 1,
        0, 1, 1, 1,
};

static const float normalsData[] = {
        // front
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,

        // back
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,
        0, 0, -1,

        // left
        -1, 0, 0,
        -1, 0, 0,
        -1, 0, 0,
        -1, 0, 0,

        // right
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,

        // top
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        // bottom
        0, -1, 0,
        0, -1, 0,
        0, -1, 0,
        0, -1, 0,
};

static const int indexData[] = {
        // front
        0, 1, 3,
        1, 2, 3,

        // back
        5, 4, 7,
        7, 6, 5,

        // left
        9, 8, 10,
        8, 11, 10,

        // right
        12, 13, 14,
        12, 14, 15,

        // top
        16, 19, 18,
        16, 18, 17,

        // bottom
        20, 21, 22,
        20, 22, 23
};
// clang-format on

int CubeMesh::GetTrianglesCount()
{
    return trianglesCount;
}

void CubeMesh::GetVertexes(const GLuint &vertexBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexSize + colorsSize + normalsSize, nullptr, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize, &vertexData);
}

void CubeMesh::GetColors(const GLuint &vertexBuffer)
{
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *) vertexSize);

    glBufferSubData(GL_ARRAY_BUFFER, vertexSize, colorsSize, &colorsData);
}

void CubeMesh::GetNormals(const GLuint &vertexBuffer)
{
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) (vertexSize + colorsSize));

    glBufferSubData(GL_ARRAY_BUFFER, vertexSize + colorsSize, normalsSize, &normalsData);
}

void CubeMesh::GetIndexes(const GLuint &indexBuffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize, &indexData, GL_STATIC_DRAW);
}
