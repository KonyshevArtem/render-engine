#include "plane_mesh.h"

static const long vertexSize  = sizeof(float) * 4 * 3;
static const long colorsSize  = sizeof(float) * 4 * 4;
static const long normalsSize = sizeof(float) * 4 * 3;
static const long indexesSize = sizeof(int) * 2 * 3;
static const long uvSize      = sizeof(float) * 4 * 2;

static const float vertexData[] = {
        -1, 0, 1,
        -1, 0, -1,
        1, 0, -1,
        1, 0, 1};

static const float colorsData[] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1};

static const float normalsData[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0};

static const int indexData[] = {
        0, 1, 3,
        3, 1, 2};

static const float uvData[] = {
        0, 0,
        0, 1,
        1, 1,
        1, 0};

int PlaneMesh::GetTrianglesCount()
{
    return 2;
}

void *PlaneMesh::GetVertexData()
{
    return (void *) &vertexData;
}

long PlaneMesh::GetVertexDataSize()
{
    return vertexSize;
}

void *PlaneMesh::GetColorsData()
{
    return (void *) &colorsData;
}

long PlaneMesh::GetColorsDataSize()
{
    return colorsSize;
}

void *PlaneMesh::GetNormalsData()
{
    return (void *) &normalsData;
}

long PlaneMesh::GetNormalsDataSize()
{
    return normalsSize;
}

void *PlaneMesh::GetIndexData()
{
    return (void *) &indexData;
}

long PlaneMesh::GetIndexDataSize()
{
    return indexesSize;
}

void *PlaneMesh::GetUVData()
{
    return (void *) &uvData;
}

long PlaneMesh::GetUVDataSize()
{
    return uvSize;
}
