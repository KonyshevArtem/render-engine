#if OPENGL_STUDY_EDITOR

#include "gizmos_pass.h"
#include "core_debug/debug.h"
#include "editor/hierarchy.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "vector4/vector4.h"

GizmosPass::GizmosPass()
{
    glGenFramebuffers(1, &m_Framebuffer);
}

GizmosPass::~GizmosPass()
{
    glDeleteFramebuffers(1, &m_Framebuffer);
}

void GizmosPass::Execute(Context &_context)
{
    Outline();
    Gizmos();
}

void GizmosPass::Outline() const
{
    static std::shared_ptr<Shader>    outlineBlitShader = Shader::Load("resources/shaders/outline/outlineBlit.shader", {});
    static std::shared_ptr<Texture2D> outlineTexture    = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};

    auto debugGroup = Debug::DebugGroup("Selected outline pass");

    CheckTexture(outlineTexture);

    // render selected gameObjects
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
        outlineTexture->Attach(GL_COLOR_ATTACHMENT0);

        glClear(GL_COLOR_BUFFER_BIT);
        for (const auto &renderer: Hierarchy::GetSelectedRenderers())
            renderer->Render();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    // blit to screen
    {
        int  width     = outlineTexture->GetWidth();
        int  height    = outlineTexture->GetHeight();
        auto texelSize = Vector4 {static_cast<float>(width), static_cast<float>(height), 1.0f / width, 1.0f / height}; // TODO: do automatic texelsize

        outlineBlitShader->Use();
        outlineBlitShader->SetTextureUniform("_Tex", *outlineTexture);
        outlineBlitShader->SetUniform("_Tex_TexelSize", &texelSize);
        outlineBlitShader->SetUniform("_Color", &outlineColor);
        Mesh::GetFullscreenMesh()->Draw();
    }
}

void GizmosPass::Gizmos() const
{
    static std::shared_ptr<Shader> gizmosShader = Shader::Load("resources/shaders/gizmos/gizmos.shader", {});

    auto debugGroup = Debug::DebugGroup("Gizmos pass");

    gizmosShader->Use();

    for (const auto &drawInfo: Gizmos::GetDrawInfos())
    {
        gizmosShader->SetUniform("_ModelMatrix", &drawInfo.Matrix);
        drawInfo.Primitive->Draw();
    }

    Shader::DetachCurrentShader();
}

void GizmosPass::CheckTexture(std::shared_ptr<Texture2D> &_texture) const
{
    int width  = Graphics::GetScreenWidth();
    int height = Graphics::GetScreenHeight();
    if (!_texture || _texture->GetWidth() != width || _texture->GetHeight() != height)
        _texture = Texture2D::Create(width, height);
}

#endif