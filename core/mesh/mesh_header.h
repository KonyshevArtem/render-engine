#ifndef RENDER_ENGINE_MESH_HEADER_H
#define RENDER_ENGINE_MESH_HEADER_H

#include "vector3/vector3.h"

struct MeshHeader
{
    char Name[128];
    int VertexDataSize;
    int IndicesCount;
    bool HasUV;
    bool HasNormals;
    bool HasTangents;
    Vector3 MinPoint;
    Vector3 MaxPoint;
};

#endif //RENDER_ENGINE_MESH_HEADER_H
