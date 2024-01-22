#if OPENGL_STUDY_EDITOR

#include "gizmos_pass.h"
#include "editor/hierarchy.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics/render_settings.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "texture_2d/texture_2d.h"
#include "graphics_backend_debug.h"

void GizmosPass::Execute(Context &_context)
{
    Outline();
    Gizmos();
}

void GizmosPass::Outline() const
{
    static RenderSettings             renderSettings {{{"LightMode", "Forward"}}};
    static std::shared_ptr<Material>  outlineMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/outline/outlineBlit.shader", {}));
    static std::shared_ptr<Texture2D> outlineTexture  = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};

    auto renderers = Hierarchy::GetSelectedRenderers();
    if (renderers.empty())
        return;

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Selected outline pass");

    CheckTexture(outlineTexture);

    // render selected gameObjects
    {
        Graphics::SetRenderTargets(outlineTexture, 0, 0, nullptr, 0, 0);

        GraphicsBackend::Clear(ClearMask::COLOR_DEPTH);

        std::vector<DrawCallInfo> infos;
        infos.reserve(renderers.size());
        for (const auto &renderer: renderers)
        {
            infos.push_back(DrawCallInfo {renderer->GetGeometry(),
                                          renderer->GetMaterial(),
                                          {renderer->GetModelMatrix()},
                                          renderer->GetAABB()});
        }
        Graphics::Draw(infos, renderSettings);
        Graphics::SetRenderTargets(nullptr, 0, 0, nullptr, 0, 0);
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

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Gizmos pass");

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
        _texture->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    }
}

#endif