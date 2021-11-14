#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "mesh_renderer.h"
#include "../../math/matrix4x4/matrix4x4.h"
#include "../gameObject/gameObject.h"
#include "../material/material.h"
#include "../mesh/mesh.h"
#include "../shader/shader.h"

MeshRenderer::MeshRenderer(shared_ptr<GameObject> _gameObject,
                           shared_ptr<Mesh>       _mesh,
                           shared_ptr<Material>   _material) :
    Renderer(std::move(_gameObject)),
    m_Mesh(std::move(_mesh)),
    m_Material(std::move(_material))
{
}

void MeshRenderer::Render()
{
    Render(m_Material->GetShader());
}

void MeshRenderer::Render(const shared_ptr<Shader> &_shader)
{
    _shader->Use();

    Matrix4x4 modelMatrix       = Matrix4x4::TRS(m_GameObject->LocalPosition, m_GameObject->LocalRotation, m_GameObject->LocalScale);
    Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

    _shader->SetUniform("_ModelMatrix", &modelMatrix);
    _shader->SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

    m_Material->TransferUniforms();
    m_Mesh->Draw();

    Shader::DetachCurrentShader();
}
