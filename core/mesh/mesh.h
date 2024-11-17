#ifndef RENDER_ENGINE_MESH_H
#define RENDER_ENGINE_MESH_H

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"

#include <memory>
#include <string>

struct Vector2;
struct Vector3;
class Material;

class Mesh: public DrawableGeometry
{
public:
    Mesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices, const std::string& name);
    Mesh(const std::vector<Vector3>& vertices,
         const std::vector<Vector3>& normals,
         const std::vector<int>& indexes,
         const std::vector<Vector2>& uvs,
         const std::vector<Vector3>& tangents,
         const std::string& name);
    ~Mesh() override = default;

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
    Bounds m_Bounds;
};


#endif //RENDER_ENGINE_MESH_H