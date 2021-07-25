#ifndef OPENGL_STUDY_CYLINDER_MESH_H
#define OPENGL_STUDY_CYLINDER_MESH_H

#include "../mesh.h"
#include "vector"

class CylinderMesh: public virtual Mesh
{
public:
    int GetTrianglesCount() override;

    CylinderMesh();

protected:
    void *GetVertexData() override;
    long  GetVertexDataSize() override;

    void *GetColorsData() override;
    long  GetColorsDataSize() override;

    void *GetNormalsData() override;
    long  GetNormalsDataSize() override;

    void *GetIndexData() override;
    long  GetIndexDataSize() override;

    void *GetUVData() override;
    long GetUVDataSize() override;

private:
    std::vector<float> vertexes;
    std::vector<float> colors;
    std::vector<float> normals;
    std::vector<int>   indexes;

    void AddVertex(float x, float y, float z);
    void AddColor(float r, float g, float b, float a);
    void AddNormal(float x, float y, float z);
    void AddTriangle(int v1, int v2, int v3);
};


#endif//OPENGL_STUDY_CYLINDER_MESH_H
