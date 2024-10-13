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

#include <memory>

void CheckTexture(std::shared_ptr<Texture2D> &_texture)
{
    int width  = Graphics::GetScreenWidth();
    int height = Graphics::GetScreenHeight();
    if (!_texture || _texture->GetWidth() != width || _texture->GetHeight() != height)
    {
        _texture = Texture2D::Create(width, height, TextureInternalFormat::SRGB_ALPHA, true);
        _texture->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    }
}

void SelectionOutlinePass::Execute(Context &_context)
{
    static std::shared_ptr<Material>  blitMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/outline/outlineBlit", {}, {}, {}, {false, DepthFunction::ALWAYS}));
    static std::shared_ptr<Material>  outlineMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/outline/silhouette", {}, {}, {}, {false, DepthFunction::ALWAYS}));
    static std::shared_ptr<Texture2D> outlineTexture  = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};
    static GraphicsBackendRenderTargetDescriptor colorTarget { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
    static GraphicsBackendRenderTargetDescriptor depthTarget { FramebufferAttachment::DEPTH_ATTACHMENT };

    auto selectedGameObjects = Hierarchy::GetSelectedGameObjects();
    if (selectedGameObjects.empty())
        return;

    CheckTexture(outlineTexture);

    // render selected gameObjects
    {
        Graphics::SetRenderTarget(colorTarget, outlineTexture);
        Graphics::SetRenderTarget(depthTarget);

        GraphicsBackend::Current()->BeginRenderPass();
        {
            auto debugGroup = GraphicsBackendDebugGroup("Selected outline pass");

            for (const auto &go: selectedGameObjects)
            {
                if (!go || !go->Renderer)
                {
                    continue;
                }

                const auto &renderer = go->Renderer;
                const auto &geometry = renderer->GetGeometry();
                const auto &material = renderer->GetMaterial();

                if (geometry && material)
                {
                    if (material->GetShader()->SupportInstancing())
                    {
                        Graphics::DrawInstanced(*geometry, *material, {renderer->GetModelMatrix()}, 0);
                    }
                    else
                    {
                        Graphics::Draw(*geometry, *outlineMaterial, renderer->GetModelMatrix(), 0);
                    }
                }
            }
        }
        GraphicsBackend::Current()->EndRenderPass();

        Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
        Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());
    }

    // blit to screen
    {
        blitMaterial->SetVector("_Color", outlineColor);

        Graphics::Blit(outlineTexture, nullptr, GraphicsBackendRenderTargetDescriptor::ColorBackbuffer(), *blitMaterial);
    }
}

#endif
