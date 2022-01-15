#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#include <OpenGL/gl3.h>
#include <vector>

class Vector2;
class Vector3;

using namespace std;

class Mesh
{
public:
    Mesh(vector<Vector3> &_vertices,
         vector<Vector3> &_normals,
         vector<int> &    _indexes,
         vector<Vector2> &_uvs,
         vector<Vector3> &_tangents);

    void Init();
    void Draw() const;

    ~Mesh();

private:
    Mesh(const Mesh &) = delete;
    Mesh(Mesh &&)      = delete;

    Mesh &operator()(const Mesh &) = delete;
    Mesh &operator()(Mesh &&)      = delete;

    GLuint m_VertexArrayObject = 0;
    GLuint m_VertexBuffer      = 0;
    GLuint m_IndexBuffer       = 0;

    vector<Vector3> m_Vertices;
    vector<Vector3> m_Normals;
    vector<Vector3> m_Tangents;
    vector<int>     m_Indexes;
    vector<Vector2> m_UVs;
};


#endif //OPENGL_STUDY_MESH_H