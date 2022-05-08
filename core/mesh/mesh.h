#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>
#include <vector>

struct Vector2;
struct Vector3;
class Material;

class Mesh: public DrawableGeometry
{
public:
    Mesh(std::vector<Vector3> &_vertices,
         std::vector<Vector3> &_normals,
         std::vector<int>     &_indexes,
         std::vector<Vector2> &_uvs,
         std::vector<Vector3> &_tangents);
    virtual ~Mesh();

    Bounds GetBounds() const;

    inline GLenum GetGeometryType() const override
    {
        return GL_TRIANGLES;
    }

    inline bool HasIndexes() const override
    {
        return true;
    }

    inline GLsizei GetElementsCount() const override
    {
        return m_Indexes.size();
    }

    static const std::shared_ptr<Mesh> &GetFullscreenMesh();

private:
    Mesh(const Mesh &) = delete;
    Mesh(Mesh &&)      = delete;

    Mesh &operator=(const Mesh &) = delete;
    Mesh &operator=(Mesh &&) = delete;

    GLuint m_IndexBuffer = 0;

    std::vector<Vector3> m_Vertices;
    std::vector<Vector3> m_Normals;
    std::vector<Vector3> m_Tangents;
    std::vector<int>     m_Indexes;
    std::vector<Vector2> m_UVs;

    Bounds m_Bounds;
};


#endif //OPENGL_STUDY_MESH_H