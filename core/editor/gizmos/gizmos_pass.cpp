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
#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"
#include "enums/clear_mask.h"
#include "shader/shader.h"

void GizmosPass::Execute(Context &_context)
{
    Outline();
    Gizmos();
}

void GizmosPass::Outline() const
{
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

        for (const auto &renderer: renderers)
        {
            const auto &geometry = renderer->GetGeometry();
            const auto &material = renderer->GetMaterial();

            if (geometry && material)
            {
                Graphics::Draw(*geometry, *material, renderer->GetModelMatrix(), 0);
            }
        }

        Graphics::SetRenderTargets(nullptr, 0, 0, nullptr, 0, 0);
    }

    // blit to screen
    {
        outlineMaterial->SetTexture("_Tex", outlineTexture);
        outlineMaterial->SetVector("_Color", outlineColor);

        Graphics::Draw(*Mesh::GetFullscreenMesh(), *outlineMaterial, Matrix4x4::Identity(), 0);
    }
}

void GizmosPass::Gizmos() const
{
    static auto gizmosMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/gizmos/gizmos.shader", {}));

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Gizmos pass");

    const auto &gizmos = Gizmos::GetGizmosToDraw();
    for (const auto &pair : gizmos)
    {
        Graphics::Draw(*pair.first, *gizmosMaterial, pair.second, 0);
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