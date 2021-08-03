#ifndef OPENGL_STUDY_PLANE_MESH_H
#define OPENGL_STUDY_PLANE_MESH_H

#include "../mesh.h"

class PlaneMesh: public virtual Mesh
{
public:
    int GetTrianglesCount() override;

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
};

#endif //OPENGL_STUDY_PLANE_MESH_H
