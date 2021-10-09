#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#include "OpenGL/gl3.h"
#include "vector"

class Vector2;
class Vector3;

using namespace std;

class Mesh
{
public:
    Mesh(const vector<Vector3> &_vertices,
         const vector<Vector3> &_normals,
         const vector<int>     &_indexes,
         const vector<Vector2> &_uvs);

    void Init();
    void Draw() const;

    virtual ~Mesh();

private:
    GLuint m_VertexArrayObject = 0;
    GLuint m_VertexBuffer      = 0;
    GLuint m_IndexBuffer       = 0;

    vector<Vector3> m_Vertices;
    vector<Vector3> m_Normals;
    vector<int>     m_Indexes;
    vector<Vector2> m_UVs;
};


#endif //OPENGL_STUDY_MESH_H