#if OPENGL_STUDY_EDITOR

#include "gizmos_pass.h"
#include "core_debug/debug.h"
#include "editor/hierarchy.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/draw_call_info.h"
#include "graphics/graphics.h"
#include "graphics/render_settings.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "vector4/vector4.h"

GizmosPass::GizmosPass()
{
    CHECK_GL(glGenFramebuffers(1, &m_Framebuffer));
}

GizmosPass::~GizmosPass()
{
    CHECK_GL(glDeleteFramebuffers(1, &m_Framebuffer));
}

void GizmosPass::Execute(Context &_context)
{
    Outline();
    Gizmos();
}

void GizmosPass::Outline() const
{
    static RenderSettings             renderSettings;
    static std::shared_ptr<Material>  outlineMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/outline/outlineBlit.shader", {}));
    static std::shared_ptr<Texture2D> outlineTexture  = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};

    auto debugGroup = Debug::DebugGroup("Selected outline pass");

    CheckTexture(outlineTexture);

    // render selected gameObjects
    {
        CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer));
        outlineTexture->Attach(GL_COLOR_ATTACHMENT0);

        CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        std::vector<DrawCallInfo> infos;
        for (const auto &renderer: Hierarchy::GetSelectedRenderers())
        {
            infos.push_back(DrawCallInfo {renderer->GetGeometry(),
                                          renderer->GetMaterial(),
                                          {renderer->GetModelMatrix()},
                                          renderer->GetAABB()});
        }
        Graphics::Draw(infos, renderSettings);

        CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    }

    // blit to screen
    {
        outlineMaterial->SetTexture("_Tex", outlineTexture);
        outlineMaterial->SetVector("_Color", outlineColor);

        auto info = DrawCallInfo {Mesh::GetFullscreenMesh(), outlineMaterial};
        Graphics::Draw(std::vector<DrawCallInfo> {info}, renderSettings);
    }
}

void GizmosPass::Gizmos() const
{
    static std::shared_ptr<Shader> gizmosShader = Shader::Load("resources/shaders/gizmos/gizmos.shader", {});

    auto debugGroup = Debug::DebugGroup("Gizmos pass");

    for (const auto &drawInfo: Gizmos::GetDrawInfos())
    {
        Shader::SetGlobalMatrix("_ModelMatrix", drawInfo.Matrix);
        gizmosShader->Use(0);
        drawInfo.Primitive->Draw();
    }
}

void GizmosPass::CheckTexture(std::shared_ptr<Texture2D> &_texture) const
{
    int width  = Graphics::GetScreenWidth();
    int height = Graphics::GetScreenHeight();
    if (!_texture || _texture->GetWidth() != width || _texture->GetHeight() != height)
    {
        _texture = Texture2D::Create(width, height);
        _texture->SetWrapMode(GL_CLAMP_TO_EDGE);
    }
}

#endif