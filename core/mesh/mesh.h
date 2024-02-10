#ifndef OPENGL_STUDY_MESH_H
#define OPENGL_STUDY_MESH_H

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"

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
    ~Mesh() override;

    inline Bounds GetBounds() const
    {
        return m_Bounds;
    }

    static const std::shared_ptr<Mesh> &GetFullscreenMesh();

    Mesh(const Mesh &) = delete;
    Mesh(Mesh &&)      = delete;

    Mesh &operator=(const Mesh &) = delete;
    Mesh &operator=(Mesh &&) = delete;

private:
    GraphicsBackendBuffer m_IndexBuffer{};

    Bounds m_Bounds;
};


#endif //OPENGL_STUDY_MESH_H