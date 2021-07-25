#include "cylinder_mesh.h"

CylinderMesh::CylinderMesh()
{
    const int baseCircleVertexes = 10;

    AddVertex(0, -1, 0);
    AddNormal(0, -1, 0);

    for (int i = 0; i < baseCircleVertexes; ++i)
    {
        float rad = (float) i / baseCircleVertexes * 2 * M_PI;
        float x   = cosf(rad);
        float z   = sinf(rad);
        AddVertex(x, -1, z);
        AddNormal(0, -1, 0);
        AddTriangle(0, (i + 1) % baseCircleVertexes + 1, i % baseCircleVertexes + 1);
    }

    int offset = baseCircleVertexes + 1;
    AddVertex(0, 1, 0);
    AddNormal(0, 1, 0);

    for (int i = 0; i < baseCircleVertexes; ++i)
    {
        float rad = (float) i / baseCircleVertexes * 2 * M_PI;
        float x   = cosf(rad);
        float z   = sinf(rad);
        AddVertex(x, 1, z);
        AddNormal(0, 1, 0);
        AddTriangle(offset, offset + i % baseCircleVertexes + 1, offset + (i + 1) % baseCircleVertexes + 1);
    }

    offset = baseCircleVertexes * 2 + 2;
    for (int i = 0; i < baseCircleVertexes; ++i)
    {
        float rad = (float) i / baseCircleVertexes * 2 * M_PI;
        float x   = cosf(rad);
        float z   = sinf(rad);
        AddVertex(x, -1, z);
        AddVertex(x, 1, z);
        AddNormal(x, 0, z);
        AddNormal(x, 0, z);

        int curr = (i % baseCircleVertexes) * 2;
        int next = ((i + 1) % baseCircleVertexes) * 2;
        AddTriangle(offset + curr, offset + next, offset + curr + 1);
        AddTriangle(offset + next, offset + next + 1, offset + curr + 1);
    }

    for (int i = 0; i < vertexes.size(); ++i)
        AddColor(1, 1, 1, 1);
}

int CylinderMesh::GetTrianglesCount()
{
    return (int) indexes.size() / 3;
}

void *CylinderMesh::GetVertexData()
{
    return vertexes.data();
}

long CylinderMesh::GetVertexDataSize()
{
    return sizeof(float) * vertexes.size();// NOLINT(cppcoreguidelines-narrowing-conversions)
}

void *CylinderMesh::GetColorsData()
{
    return colors.data();
}

long CylinderMesh::GetColorsDataSize()
{
    return sizeof(float) * colors.size();// NOLINT(cppcoreguidelines-narrowing-conversions)
}

void *CylinderMesh::GetNormalsData()
{
    return normals.data();
}

long CylinderMesh::GetNormalsDataSize()
{
    return sizeof(float) * normals.size();// NOLINT(cppcoreguidelines-narrowing-conversions)
}

void *CylinderMesh::GetIndexData()
{
    return indexes.data();
}

long CylinderMesh::GetIndexDataSize()
{
    return sizeof(int) * indexes.size();// NOLINT(cppcoreguidelines-narrowing-conversions)
}

void CylinderMesh::AddVertex(float x, float y, float z)
{
    vertexes.push_back(x);
    vertexes.push_back(y);
    vertexes.push_back(z);
}

void CylinderMesh::AddColor(float r, float g, float b, float a)
{
    colors.push_back(r);
    colors.push_back(g);
    colors.push_back(b);
    colors.push_back(a);
}

void CylinderMesh::AddNormal(float x, float y, float z)
{
    normals.push_back(x);
    normals.push_back(y);
    normals.push_back(z);
}

void CylinderMesh::AddTriangle(int v1, int v2, int v3)
{
    indexes.push_back(v1);
    indexes.push_back(v2);
    indexes.push_back(v3);
}

void *CylinderMesh::GetUVData()
{
    return nullptr;
}

long CylinderMesh::GetUVDataSize()
{
    return 0;
}
