#if RENDER_ENGINE_EDITOR

#include "selection_outline_pass.h"
#include "material/material.h"
#include "shader/shader.h"
#include "editor/hierarchy.h"
#include "texture_2d/texture_2d.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"
#include "graphics/graphics.h"
#include "enums/clear_mask.h"
#include "renderer/renderer.h"
#include "gameObject/gameObject.h"

#include <memory>

void CheckTexture(std::shared_ptr<Texture2D> &_texture)
{
    int width  = Graphics::GetScreenWidth();
    int height = Graphics::GetScreenHeight();
    if (!_texture || _texture->GetWidth() != width || _texture->GetHeight() != height)
    {
        _texture = Texture2D::Create(width, height, TextureInternalFormat::SRGB_ALPHA, TexturePixelFormat::RGBA, TextureDataType::UNSIGNED_BYTE);
        _texture->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    }
}

void SelectionOutlinePass::Execute(Context &_context)
{
    static std::shared_ptr<Material>  outlineMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/outline/outlineBlit.shader", {}));
    static std::shared_ptr<Texture2D> outlineTexture  = nullptr;
    static Vector4                    outlineColor {1, 0.73f, 0, 1};

    auto selectedGameObjects = Hierarchy::GetSelectedGameObjects();
    if (selectedGameObjects.empty())
        return;

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Selected outline pass");

    CheckTexture(outlineTexture);

    // render selected gameObjects
    {
        Graphics::SetRenderTargets(outlineTexture, 0, 0, nullptr, 0, 0);

        GraphicsBackend::Current()->Clear(ClearMask::COLOR_DEPTH);

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
                    Graphics::DrawInstanced(*geometry,*material, {renderer->GetModelMatrix()}, 0);
                }
                else
                {
                    Graphics::Draw(*geometry, *material, renderer->GetModelMatrix(), 0);
                }
            }
        }

        Graphics::SetRenderTargets(nullptr, 0, 0, nullptr, 0, 0);
    }

    // blit to screen
    {
        outlineMaterial->SetVector("_Color", outlineColor);

        Graphics::Blit(outlineTexture, nullptr, 0, 0, *outlineMaterial);
    }
}

#endif
