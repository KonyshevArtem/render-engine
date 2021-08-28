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
    long  GetUVDataSize() override;

private:
    std::vector<float> m_Vertexes;
    std::vector<float> m_Colors;
    std::vector<float> m_Normals;
    std::vector<int>   m_Indexes;

    void AddVertex(float _x, float _y, float _z);
    void AddColor(float _r, float _g, float _b, float _a);
    void AddNormal(float _x, float _y, float _z);
    void AddTriangle(int _v1, int _v2, int _v3);
};


#endif //OPENGL_STUDY_CYLINDER_MESH_H
