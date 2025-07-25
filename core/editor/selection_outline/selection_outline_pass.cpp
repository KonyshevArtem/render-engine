#if RENDER_ENGINE_EDITOR

#include "selection_outline_pass.h"
#include "material/material.h"
#include "shader/shader.h"
#include "editor/hierarchy.h"
#include "texture_2d/texture_2d.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug_group.h"
#include "graphics/graphics.h"
#include "renderer/renderer.h"
#include "gameObject/gameObject.h"
#include "enums/framebuffer_attachment.h"
#include "enums/load_action.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "editor/profiler/profiler.h"

void CheckTexture(std::shared_ptr<Texture2D> &_texture)
{
    const int width  = Graphics::GetScreenWidth();
    const int height = Graphics::GetScreenHeight();
    if (!_texture || _texture->GetWidth() != width || _texture->GetHeight() != height)
    {
        _texture = Texture2D::Create(width, height, TextureInternalFormat::RGBA8, false, true, "SilhouetteRT");
        _texture->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    }
}

SelectionOutlinePass::SelectionOutlinePass(int priority) :
    RenderPass(priority)
{
}

bool SelectionOutlinePass::Prepare()
{
    return !Hierarchy::GetSelectedGameObjects().empty();
}

void SelectionOutlinePass::Execute(const Context& ctx)
{
    static std::shared_ptr<Material>  blitMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/outlineBlit", {}, {}, {}, {false, DepthFunction::ALWAYS}), "OutlineBlit");
    static std::shared_ptr<Material>  silhouetteMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/silhouette", {}, {}, {}, {false, DepthFunction::ALWAYS}), "Silhouette");
    static std::shared_ptr<Texture2D> silhouetteRenderTarget  = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};
    static GraphicsBackendRenderTargetDescriptor colorTarget { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
    static GraphicsBackendRenderTargetDescriptor depthTarget { FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT };

    Profiler::Marker marker("SelectionOutlinePass::Execute");

    const std::unordered_set<std::shared_ptr<GameObject>>& selectedGameObjects = Hierarchy::GetSelectedGameObjects();

    CheckTexture(silhouetteRenderTarget);

    // render selected gameObjects
    {
        Profiler::GPUMarker gpuMarker("Selection Outline Pass");

        Graphics::SetRenderTarget(colorTarget, silhouetteRenderTarget);
        Graphics::SetRenderTarget(depthTarget);

        GraphicsBackend::Current()->BeginRenderPass("Selection Outline Pass");
        for (const auto &go: selectedGameObjects)
        {
            if (!go || !go->GetRenderer())
            {
                continue;
            }

            const auto &renderer = go->GetRenderer();
            const auto &geometry = renderer->GetGeometry();
            const auto &material = renderer->GetMaterial();

            if (geometry && material)
            {
                if (material->GetShader()->SupportInstancing())
                {
                    Graphics::DrawInstanced(*geometry, *material, {renderer->GetModelMatrix()});
                }
                else
                {
                    Graphics::Draw(*geometry, *silhouetteMaterial, renderer->GetModelMatrix());
                }
            }
        }
        GraphicsBackend::Current()->EndRenderPass();

        Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
        Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());
    }

    // blit to screen
    {
        Profiler::GPUMarker gpuMarker("Selection Blit Pass");

        blitMaterial->SetVector("_Color", outlineColor);

        Graphics::Blit(silhouetteRenderTarget, nullptr, GraphicsBackendRenderTargetDescriptor::ColorBackbuffer(), *blitMaterial, "Selection Blit Pass");
    }
}

#endif
