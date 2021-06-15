#define GL_SILENCE_DEPRECATION

#include "cube_mesh.h"


static const int  vertexCount    = 24;
static const int  trianglesCount = 12;
static const long vertexSize     = sizeof(float) * vertexCount * 3;
static const long colorsSize     = sizeof(float) * vertexCount * 4;
static const long normalsSize    = sizeof(float) * vertexCount * 3;
static const long indexesSize    = sizeof(int) * trianglesCount * 3;

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

void *CubeMesh::GetVertexData()
{
    return (void *) &vertexData;
}

long CubeMesh::GetVertexDataSize()
{
    return vertexSize;
}

void *CubeMesh::GetColorsData()
{
    return (void *) &colorsData;
}

long CubeMesh::GetColorsDataSize()
{
    return colorsSize;
}

void *CubeMesh::GetNormalsData()
{
    return (void *) &normalsData;
}

long CubeMesh::GetNormalsDataSize()
{
    return normalsSize;
}

void *CubeMesh::GetIndexData()
{
    return (void *) &indexData;
}

long CubeMesh::GetIndexDataSize()
{
    return indexesSize;
}
