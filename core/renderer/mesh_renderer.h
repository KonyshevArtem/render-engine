#ifndef OPENGL_STUDY_MESH_RENDERER_H
#define OPENGL_STUDY_MESH_RENDERER_H

#include "memory"
#include "renderer.h"

class GameObject;
class Mesh;
class Material;
class Shader;

using namespace std;

class MeshRenderer: public Renderer
{
public:
    MeshRenderer(const shared_ptr<GameObject> &_gameObject,
                 shared_ptr<Mesh>              _mesh,
                 shared_ptr<Material>          _material);
    virtual ~MeshRenderer() = default;

    void Render() const override;

private:
    MeshRenderer(const MeshRenderer &) = delete;
    MeshRenderer(MeshRenderer &&)      = delete;

    MeshRenderer &operator()(const MeshRenderer &) = delete;
    MeshRenderer &operator()(MeshRenderer &&)      = delete;


    shared_ptr<Mesh>     m_Mesh;
    shared_ptr<Material> m_Material;

    int GetRenderQueue() const override;
};

#endif //OPENGL_STUDY_MESH_RENDERER_H
