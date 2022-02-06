#ifndef OPENGL_STUDY_MESH_RENDERER_H
#define OPENGL_STUDY_MESH_RENDERER_H

#include "renderer.h"
#include <memory>

class GameObject;
class Mesh;
class Material;
class Shader;
struct Bounds;

class MeshRenderer: public Renderer
{
public:
    MeshRenderer(const std::shared_ptr<GameObject> &_gameObject,
                 std::shared_ptr<Mesh>              _mesh,
                 std::shared_ptr<Material>          _material);
    virtual ~MeshRenderer() = default;

    void   Render() const override;
    Bounds GetAABB() const override;

private:
    MeshRenderer(const MeshRenderer &) = delete;
    MeshRenderer(MeshRenderer &&)      = delete;

    MeshRenderer &operator=(const MeshRenderer &) = delete;
    MeshRenderer &operator=(MeshRenderer &&)      = delete;


    std::shared_ptr<Mesh>     m_Mesh;
    std::shared_ptr<Material> m_Material;

    int GetRenderQueue() const override;
};

#endif //OPENGL_STUDY_MESH_RENDERER_H
