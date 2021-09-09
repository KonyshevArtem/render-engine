#ifndef OPENGL_STUDY_CUBE_MESH_H
#define OPENGL_STUDY_CUBE_MESH_H

#include "../mesh.h"

class CubeMesh: public virtual Mesh
{
public:
    int GetTrianglesCount() override;

protected:
    void *GetVertexData() override;
    long  GetVertexDataSize() override;

    void *GetNormalsData() override;
    long  GetNormalsDataSize() override;

    void *GetIndexData() override;
    long  GetIndexDataSize() override;

    void *GetUVData() override;
    long GetUVDataSize() override;
};


#endif//OPENGL_STUDY_CUBE_MESH_H
