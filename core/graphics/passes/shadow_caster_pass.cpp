#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "shadow_caster_pass.h"

#include "../../light/light.h"
#include "../../renderer/renderer.h"
#include "../../shader/shader.h"
#include "../../texture_2d_array/texture_2d_array.h"
#include "../context.h"
#include "../graphics.h"
#include "../uniform_block.h"
#include <utility>

ShadowCasterPass::ShadowCasterPass(int _spotLightsCount, shared_ptr<UniformBlock> _shadowsUniformBlock) :
    m_ShadowsUniformBlock(std::move(_shadowsUniformBlock)),
    m_ShadowCasterShader(Shader::Load("resources/shaders/shadowCaster/shadowCaster.shader", vector<string>())),
    m_SpotLightShadowMapArray(Texture2DArray::ShadowMapArray(SHADOW_MAP_SIZE, _spotLightsCount))
{
    Shader::SetGlobalTexture("_SpotLightShadowMapArray", m_SpotLightShadowMapArray);

    glGenFramebuffers(1, &m_Framebuffer);
}

ShadowCasterPass::~ShadowCasterPass()
{
    glDeleteFramebuffers(1, &m_Framebuffer);
}

void ShadowCasterPass::Execute(const Context &_ctx)
{
    if (m_ShadowCasterShader == nullptr)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

    int spotLightsCount = 0;
    for (const auto *light: _ctx.Lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            m_SpotLightShadowMapArray->Attach(GL_DEPTH_ATTACHMENT, spotLightsCount);

            auto view    = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            auto proj    = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, 100);
            auto lightVP = proj * view;

            Graphics::SetCameraData(view, proj);

            auto index = to_string(spotLightsCount);
            m_ShadowsUniformBlock->SetUniform("_SpotLightShadows[" + index + "].LightViewProjMatrix", &lightVP, sizeof(Matrix4x4));

            ++spotLightsCount;
        }
        else
            continue;

        Render(_ctx.Renderers);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ShadowCasterPass::Render(const vector<Renderer *> &_renderers)
{
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto *r: _renderers)
    {
        if (r != nullptr)
            r->Render(*m_ShadowCasterShader);
    }
}
