#include "mesh_renderer.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"
#include "mesh/mesh.h"
#include "shader/shader.h"

MeshRenderer::MeshRenderer(const std::shared_ptr<GameObject> &_gameObject,
                           std::shared_ptr<Mesh>              _mesh,
                           std::shared_ptr<Material>          _material) :
    Renderer(_gameObject),
    m_Mesh(std::move(_mesh)),
    m_Material(std::move(_material))
{
}

void MeshRenderer::Render(const RenderSettings &_settings) const
{
    Matrix4x4 modelMatrix = GetModelMatrix();

    Shader::SetGlobalMatrix("_ModelMatrix", modelMatrix);
    Shader::SetGlobalMatrix("_ModelNormalMatrix", modelMatrix.Invert().Transpose());

    m_Mesh->Draw(*m_Material, _settings);
}

Bounds MeshRenderer::GetAABB() const
{
    return m_Mesh ? GetModelMatrix() * m_Mesh->GetBounds() : Bounds();
}

int MeshRenderer::GetRenderQueue() const
{
    return m_Material ? m_Material->GetRenderQueue() : 2000;
}