#include "cylinder_mesh.h"

CylinderMesh::CylinderMesh()
{
    const int baseCircleVertexes = 10;

    AddVertex(0, -1, 0);
    AddNormal(0, -1, 0);

    for (int i = 0; i < baseCircleVertexes; ++i)
    {
        float rad = (float) i / baseCircleVertexes * 2 * M_PI; // NOLINT(cppcoreguidelines-narrowing-conversions)
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
        float rad = (float) i / baseCircleVertexes * 2 * M_PI; // NOLINT(cppcoreguidelines-narrowing-conversions)
        float x   = cosf(rad);
        float z   = sinf(rad);
        AddVertex(x, 1, z);
        AddNormal(0, 1, 0);
        AddTriangle(offset, offset + i % baseCircleVertexes + 1, offset + (i + 1) % baseCircleVertexes + 1);
    }

    offset = baseCircleVertexes * 2 + 2;
    for (int i = 0; i < baseCircleVertexes; ++i)
    {
        float rad = (float) i / baseCircleVertexes * 2 * M_PI; // NOLINT(cppcoreguidelines-narrowing-conversions)
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
}

int CylinderMesh::GetTrianglesCount()
{
    return (int) m_Indexes.size() / 3;
}

void *CylinderMesh::GetVertexData()
{
    return m_Vertexes.data();
}

long CylinderMesh::GetVertexDataSize()
{
    return sizeof(float) * m_Vertexes.size(); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

void *CylinderMesh::GetNormalsData()
{
    return m_Normals.data();
}

long CylinderMesh::GetNormalsDataSize()
{
    return sizeof(float) * m_Normals.size(); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

void *CylinderMesh::GetIndexData()
{
    return m_Indexes.data();
}

long CylinderMesh::GetIndexDataSize()
{
    return sizeof(int) * m_Indexes.size(); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

void CylinderMesh::AddVertex(float _x, float _y, float _z)
{
    m_Vertexes.push_back(_x);
    m_Vertexes.push_back(_y);
    m_Vertexes.push_back(_z);
}

void CylinderMesh::AddNormal(float _x, float _y, float _z)
{
    m_Normals.push_back(_x);
    m_Normals.push_back(_y);
    m_Normals.push_back(_z);
}

void CylinderMesh::AddTriangle(int _v1, int _v2, int _v3)
{
    m_Indexes.push_back(_v1);
    m_Indexes.push_back(_v2);
    m_Indexes.push_back(_v3);
}

void *CylinderMesh::GetUVData()
{
    return nullptr;
}

long CylinderMesh::GetUVDataSize()
{
    return 0;
}
