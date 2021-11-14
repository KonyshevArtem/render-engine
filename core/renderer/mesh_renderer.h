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
    MeshRenderer(shared_ptr<GameObject> _gameObject,
                 shared_ptr<Mesh>       _mesh,
                 shared_ptr<Material>   _material);

    void Render() override;
    void Render(const shared_ptr<Shader> &_shader) override;

private:
    shared_ptr<Mesh>     m_Mesh;
    shared_ptr<Material> m_Material;
};

#endif //OPENGL_STUDY_MESH_RENDERER_H
