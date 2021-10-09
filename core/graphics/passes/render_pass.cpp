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
#include "../../texture_2d_array/texture_2d_array.h"
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
        shader->BindDefaultTextures();

        auto textureUnits = shader->GetTextureUnits();
        go->Material->TransferUniforms(textureUnits);

        for (const auto &pair: _ctx->Textures2D)
        {
            if (!textureUnits.contains(pair.first))
                continue;

            int unit = textureUnits[pair.first];
            pair.second->Bind(unit);
            shader->SetUniform(pair.first, &unit);
        }

        for (const auto &pair: _ctx->Texture2DArrays)
        {
            if (!textureUnits.contains(pair.first))
                continue;

            int unit = textureUnits[pair.first];
            pair.second->Bind(unit);
            shader->SetUniform(pair.first, &unit);
        }

        go->Mesh->Draw();

        for (const auto &pair: textureUnits)
        {
            glActiveTexture(GL_TEXTURE0 + pair.second);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            glBindSampler(pair.second, 0);
        }

        glUseProgram(0);
    }
}
