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

MeshRenderer::MeshRenderer(const shared_ptr<GameObject> &_gameObject,
                           shared_ptr<Mesh>              _mesh,
                           shared_ptr<Material>          _material) :
    Renderer(_gameObject),
    m_Mesh(std::move(_mesh)),
    m_Material(std::move(_material))
{
}

void MeshRenderer::Render() const
{
    const auto& shaderPtr = m_Material->GetShader();
    if (shaderPtr != nullptr)
        Render(*shaderPtr);
}

void MeshRenderer::Render(const Shader &_shader) const
{
    if (m_GameObject.expired())
        return;

    _shader.Use();

    auto      go                = m_GameObject.lock();
    Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
    Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

    _shader.SetUniform("_ModelMatrix", &modelMatrix);
    _shader.SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

    m_Material->TransferUniforms();
    m_Mesh->Draw();

    Shader::DetachCurrentShader();
}

int MeshRenderer::GetRenderQueue() const
{
    return m_Material ? m_Material->GetRenderQueue() : 2000;
}