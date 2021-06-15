#ifndef OPENGL_STUDY_CUBE_MESH_H
#define OPENGL_STUDY_CUBE_MESH_H

#include "../mesh.h"

class CubeMesh: public virtual Mesh
{
public:
    int GetTrianglesCount() override;

protected:
    void GetVertexes(const GLuint &vertexBuffer) override;
    void GetColors(const GLuint &vertexBuffer) override;
    void GetNormals(const GLuint &vertexBuffer) override;
    void GetIndexes(const GLuint &indexBuffer) override;
};


#endif//OPENGL_STUDY_CUBE_MESH_H
