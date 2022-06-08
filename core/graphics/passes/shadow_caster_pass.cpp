#include "shadow_caster_pass.h"
#include "../context.h"
#include "../graphics.h"
#include "../uniform_block.h"
#include "bounds/bounds.h"
#include "core_debug/debug.h"
#include "graphics/draw_call_info.h"
#include "graphics/render_settings.h"
#include "light/light.h"
#include "renderer/renderer.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "texture_2d_array/texture_2d_array.h"
#include "vector4/vector4.h"
#include <utility>
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

ShadowCasterPass::ShadowCasterPass(int _spotLightsCount, std::shared_ptr<UniformBlock> _shadowsUniformBlock) :
    m_ShadowsUniformBlock(std::move(_shadowsUniformBlock)),
    m_SpotLightShadowMapArray(Texture2DArray::ShadowMapArray(SPOT_LIGHT_SHADOW_MAP_SIZE, _spotLightsCount)),
    m_DirectionLightShadowMap(Texture2D::CreateShadowMap(DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE))
{
    Shader::SetGlobalTexture("_DirLightShadowMap", m_DirectionLightShadowMap);
    Shader::SetGlobalTexture("_SpotLightShadowMapArray", m_SpotLightShadowMapArray);

    m_DirectionLightShadowMap->SetBorderColor({1, 1, 1, 1});
    m_DirectionLightShadowMap->SetWrapMode(GL_CLAMP_TO_BORDER);
}

void ShadowCasterPass::Execute(const Context &_ctx)
{
    static const Matrix4x4   biasMatrix         = Matrix4x4::TRS(Vector3 {0.5f, 0.5f, 0.5f}, Quaternion(), Vector3 {0.5f, 0.5f, 0.5f});
    static const std::string dirLightMatrixName = "_DirLightShadow.LightViewProjMatrix";

    static bool        namesInited = false;
    static std::string spotLightNames[Graphics::MAX_SPOT_LIGHT_SOURCES];

    if (!namesInited)
    {
        for (int i = 0; i < Graphics::MAX_SPOT_LIGHT_SOURCES; ++i)
            spotLightNames[i] = "_SpotLightShadows[" + std::to_string(i) + "].LightViewProjMatrix";
        namesInited = true;
    }

    /// ----- ///

    if (_ctx.ShadowCasters.size() == 0)
        return;

    auto debugGroup = Debug::DebugGroup("Shadow pass");

    int spotLightsCount = 0;
    for (const auto *light: _ctx.Lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            CHECK_GL(glViewport(0, 0, SPOT_LIGHT_SHADOW_MAP_SIZE, SPOT_LIGHT_SHADOW_MAP_SIZE));

            m_SpotLightShadowMapArray->AttachmentLayer = spotLightsCount;
            Graphics::SetRenderTargets(nullptr, m_SpotLightShadowMapArray);

            auto view    = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            auto proj    = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, _ctx.ShadowDistance);
            auto lightVP = biasMatrix * proj * view;

            Graphics::SetCameraData(view, proj);
            m_ShadowsUniformBlock->SetUniform(spotLightNames[spotLightsCount], &lightVP, sizeof(Matrix4x4));

            ++spotLightsCount;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            CHECK_GL(glViewport(0, 0, DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE));

            Graphics::SetRenderTargets(nullptr, m_DirectionLightShadowMap);

            auto bounds = _ctx.ShadowCasters[0]->GetAABB();
            for (const auto &renderer: _ctx.ShadowCasters)
                bounds = bounds.Combine(renderer->GetAABB());

            auto extentsWorldSpace   = bounds.GetExtents();
            auto maxExtentWorldSpace = std::max({extentsWorldSpace.x, extentsWorldSpace.y, extentsWorldSpace.z});
            auto lightDir            = light->Rotation * Vector3 {0, 0, 1};
            auto viewPos             = bounds.GetCenter() - lightDir * maxExtentWorldSpace;
            auto viewMatrix          = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-viewPos);

            auto boundsViewSpace    = viewMatrix * bounds;
            auto extentsViewSpace   = boundsViewSpace.GetExtents();
            auto maxExtentViewSpace = std::max(extentsViewSpace.x, extentsViewSpace.y);
            auto projMatrix         = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0, _ctx.ShadowDistance);

            Graphics::SetCameraData(viewMatrix, projMatrix);

            auto lightVP = biasMatrix * projMatrix * viewMatrix;
            m_ShadowsUniformBlock->SetUniform(dirLightMatrixName, &lightVP, sizeof(Matrix4x4));
        }
        else
            continue;

        Render(_ctx.ShadowCasters);
    }

    m_ShadowsUniformBlock->UploadData();

    Graphics::SetRenderTargets(nullptr, nullptr);
}

void ShadowCasterPass::Render(const std::vector<Renderer *> &_renderers)
{
    static RenderSettings renderSettings {{{"LightMode", "ShadowCaster"}}};

    auto debugGroup = Debug::DebugGroup("Render shadow map");

    CHECK_GL(glDepthMask(GL_TRUE));
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    auto drawCalls = Graphics::DoCulling(_renderers);
    Graphics::Draw(drawCalls, renderSettings);
}
