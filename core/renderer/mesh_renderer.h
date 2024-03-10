#ifndef RENDER_ENGINE_MESH_RENDERER_H
#define RENDER_ENGINE_MESH_RENDERER_H

#include "renderer.h"
#include <memory>

class Mesh;

class MeshRenderer: public Renderer
{
public:
    MeshRenderer(const std::shared_ptr<GameObject> &_gameObject,
                 std::shared_ptr<Mesh>              _mesh,
                 const std::shared_ptr<Material>    &_material);
    ~MeshRenderer() override = default;

    Bounds                            GetAABB() const override;
    std::shared_ptr<DrawableGeometry> GetGeometry() const override;

    MeshRenderer(const MeshRenderer &) = delete;
    MeshRenderer(MeshRenderer &&)      = delete;

    MeshRenderer &operator=(const MeshRenderer &) = delete;
    MeshRenderer &operator=(MeshRenderer &&) = delete;

private:
    std::shared_ptr<Mesh> m_Mesh;
};

#endif //RENDER_ENGINE_MESH_RENDERER_H
