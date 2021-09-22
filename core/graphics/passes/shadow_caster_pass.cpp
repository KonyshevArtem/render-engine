#include "shadow_caster_pass.h"
#include "../graphics.h"

ShadowCasterPass::ShadowCasterPass(int _spotLightsCount, const shared_ptr<UniformBlock> &_shadowsUniformBlock)
{
    m_ShadowsUniformBlock     = _shadowsUniformBlock;
    m_ShadowCasterShader      = Shader::Load("shaders/shadowCaster.glsl", vector<string>());
    m_SpotLightShadowMapArray = Texture2DArray::ShadowMapArray(SHADOW_MAP_SIZE, _spotLightsCount);

    glGenFramebuffers(1, &m_Framebuffer);
}

ShadowCasterPass::~ShadowCasterPass()
{
    glDeleteFramebuffers(1, &m_Framebuffer);
}

void ShadowCasterPass::Execute(shared_ptr<Context> &_ctx)
{
    if (m_ShadowCasterShader == nullptr)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_SpotLightShadowMapArray->m_Texture);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

    int spotLightsCount = 0;
    for (const auto &light: _ctx->Lights)
    {
        if (light->Type == SPOT)
        {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_SpotLightShadowMapArray->m_Texture, 0, spotLightsCount);

            Matrix4x4 view    = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            Matrix4x4 proj    = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, 100);
            Matrix4x4 lightVP = proj * view;

            Graphics::SetCameraData(view, proj);

            string index = to_string(spotLightsCount);
            m_ShadowsUniformBlock->SetUniform("_SpotLightShadows[" + index + "].LightViewProjMatrix", &lightVP, sizeof(Matrix4x4));

            ++spotLightsCount;
        }
        else
            continue;

        Render(_ctx->GameObjects);
    }

    if (spotLightsCount > 0)
        _ctx->Texture2DArrays["_SpotLightShadowMapArray"] = m_SpotLightShadowMapArray;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ShadowCasterPass::Render(const vector<shared_ptr<GameObject>> &_gameObjects)
{
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_ShadowCasterShader->m_Program);

    for (const auto &go: _gameObjects)
    {
        glBindVertexArray(go->Mesh->m_VertexArrayObject);

        Matrix4x4 modelMatrix = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        m_ShadowCasterShader->SetUniform("_ModelMatrix", &modelMatrix);

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    glUseProgram(0);
}