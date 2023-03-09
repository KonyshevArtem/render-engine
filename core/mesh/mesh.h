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
    Mesh(const std::vector<Vector3> &_vertices,
         const std::vector<Vector3> &_normals,
         const std::vector<int>     &_indexes,
         const std::vector<Vector2> &_uvs,
         const std::vector<Vector3> &_tangents);
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
        return m_IndicesCount;
    }

    static const std::shared_ptr<Mesh> &GetFullscreenMesh();

private:
    Mesh(const Mesh &) = delete;
    Mesh(Mesh &&)      = delete;

    Mesh &operator=(const Mesh &) = delete;
    Mesh &operator=(Mesh &&) = delete;

    GLuint m_IndexBuffer = 0;

    Bounds m_Bounds;
    int m_IndicesCount;
};


#endif //OPENGL_STUDY_MESH_H