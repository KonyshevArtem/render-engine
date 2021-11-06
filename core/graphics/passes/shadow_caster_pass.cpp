#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "shadow_caster_pass.h"

#include <utility>
#include "../../gameObject/gameObject.h"
#include "../../light/light.h"
#include "../../mesh/mesh.h"
#include "../../shader/shader.h"
#include "../../texture_2d_array/texture_2d_array.h"
#include "../context.h"
#include "../graphics.h"
#include "../uniform_block.h"

ShadowCasterPass::ShadowCasterPass(int _spotLightsCount, shared_ptr<UniformBlock> _shadowsUniformBlock)
{
    m_ShadowsUniformBlock     = std::move(_shadowsUniformBlock);
    m_ShadowCasterShader      = Shader::Load("resources/shaders/shadowCaster.glsl", vector<string>());
    m_SpotLightShadowMapArray = Texture2DArray::ShadowMapArray(SHADOW_MAP_SIZE, _spotLightsCount);

    Shader::SetGlobalTexture("_SpotLightShadowMapArray", m_SpotLightShadowMapArray);

    glGenFramebuffers(1, &m_Framebuffer);
}

ShadowCasterPass::~ShadowCasterPass()
{
    glDeleteFramebuffers(1, &m_Framebuffer);
}

void ShadowCasterPass::Execute(const shared_ptr<Context> &_ctx)
{
    if (m_ShadowCasterShader == nullptr)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

    int spotLightsCount = 0;
    for (const auto &light: _ctx->Lights)
    {
        if (light->Type == LightType::SPOT)
        {
            m_SpotLightShadowMapArray->Attach(GL_DEPTH_ATTACHMENT, spotLightsCount);

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ShadowCasterPass::Render(const vector<shared_ptr<GameObject>> &_gameObjects)
{
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ShadowCasterShader->Use();

    for (const auto &go: _gameObjects)
    {
        if (go->Mesh == nullptr)
            continue;

        Matrix4x4 modelMatrix = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        m_ShadowCasterShader->SetUniform("_ModelMatrix", &modelMatrix);
        go->Mesh->Draw();
    }

    Shader::DetachCurrentShader();
}
