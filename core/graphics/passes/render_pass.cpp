#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "render_pass.h"
#include "../../gameObject/gameObject.h"
#include "../../material/material.h"
#include "../../mesh/mesh.h"
#include "../../shader/shader.h"
#include "../../texture_2d/texture_2d.h"
#include "../context.h"
#include "../graphics.h"

RenderPass::RenderPass()
{
}

void RenderPass::Execute(shared_ptr<Context> &_ctx)
{
    glViewport(0, 0, Graphics::ScreenWidth, Graphics::ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Graphics::SetCameraData(_ctx->ViewMatrix, _ctx->ProjectionMatrix);

    for (const auto &go: _ctx->GameObjects)
    {
        if (go->Mesh == nullptr || go->Material == nullptr)
            continue;

        auto shader = go->Material->GetShader();

        shader->Use();

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        shader->SetUniform("_ModelMatrix", &modelMatrix);
        shader->SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

        go->Material->TransferUniforms();
        go->Mesh->Draw();

        Shader::DetachCurrentShader();
    }
}
