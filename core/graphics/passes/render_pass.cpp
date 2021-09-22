#include "render_pass.h"
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

        auto shader = go->Material->m_Shader != nullptr ? go->Material->m_Shader : Shader::FallbackShader;

        glUseProgram(shader->m_Program);
        glBindVertexArray(go->Mesh->m_VertexArrayObject);

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        shader->SetUniform("_ModelMatrix", &modelMatrix);
        shader->SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

        unordered_map<string, int> textureUnits;
        BindDefaultTextures(shader, textureUnits);
        TransferUniformsFromMaterial(go->Material, textureUnits);

        for (const auto &pair: _ctx->Textures2D)
        {
            if (!textureUnits.contains(pair.first))
                continue;

            int unit = textureUnits[pair.first];
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, pair.second->m_Texture);
            glBindSampler(unit, pair.second->m_Sampler);
            shader->SetUniform(pair.first, &unit);
        }

        for (const auto &pair: _ctx->Texture2DArrays)
        {
            if (!textureUnits.contains(pair.first))
                continue;

            int unit = textureUnits[pair.first];
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, pair.second->m_Texture);
            glBindSampler(unit, pair.second->m_Sampler);
            shader->SetUniform(pair.first, &unit);
        }

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        for (const auto &pair: textureUnits)
        {
            glActiveTexture(GL_TEXTURE0 + pair.second);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            glBindSampler(pair.second, 0);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void RenderPass::BindDefaultTextures(const shared_ptr<Shader> &_shader, unordered_map<string, int> &_textureUnits)
{
    shared_ptr<Texture2D> white = Texture2D::White();

    int unit = 0;
    for (const auto &pair: _shader->m_Uniforms)
    {
        if (pair.second.Type == SAMPLER_2D)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, white->m_Texture);
            glBindSampler(unit, white->m_Sampler);

            Vector4 st = Vector4(0, 0, 1, 1);
            _shader->SetUniform(pair.first, &unit);
            _shader->SetUniform(pair.first + "ST", &st);

            _textureUnits[pair.first] = unit++;
        }
        else if (pair.second.Type == SAMPLER_2D_ARRAY)
        {
            // TODO: bind default texture array
            _textureUnits[pair.first] = unit++;
        }
    }
}

void RenderPass::TransferUniformsFromMaterial(const shared_ptr<Material> &_material, const unordered_map<string, int> &_textureUnits)
{
    for (const auto &pair: _material->m_Textures2D)
    {
        if (pair.second == nullptr || !_textureUnits.contains(pair.first))
            continue;

        int unit = _textureUnits.at(pair.first);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, pair.second->m_Texture);
        glBindSampler(unit, pair.second->m_Sampler);

        _material->m_Shader->SetUniform(pair.first, &unit);
    }

    for (const auto &pair: _material->m_Vectors4)
        _material->m_Shader->SetUniform(pair.first, &pair.second);
    for (const auto &pair: _material->m_Floats)
        _material->m_Shader->SetUniform(pair.first, &pair.second);
}
