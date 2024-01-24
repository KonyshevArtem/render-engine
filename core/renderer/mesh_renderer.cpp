#include "mesh_renderer.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "shader/shader.h"

MeshRenderer::MeshRenderer(const std::shared_ptr<GameObject> &_gameObject,
                           std::shared_ptr<Mesh>              _mesh,
                           const std::shared_ptr<Material>   &_material) :
    Renderer(_gameObject, _material),
    m_Mesh(std::move(_mesh))
{
}

Bounds MeshRenderer::GetAABB() const
{
    return m_Mesh ? GetModelMatrix() * m_Mesh->GetBounds() : Bounds();
}

std::shared_ptr<DrawableGeometry> MeshRenderer::GetGeometry() const
{
    return m_Mesh;
}