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
#include "vector2/vector2.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"
#include "enums/indices_data_type.h"
#include "graphics/context.h"
#include "graphics/render_settings/render_settings.h"

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
    m_SilhouetteMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/silhouette", {}, {}, {}, {false, ComparisonFunction::ALWAYS}), "Silhouette");
}

bool SelectionOutlinePass::Prepare(const Context& ctx)
{
    static std::vector<std::shared_ptr<Renderer>> selectedRenderers;

    const std::unordered_set<std::shared_ptr<GameObject>>& selectedGameObjects = Hierarchy::GetSelectedGameObjects();
    if (selectedGameObjects.empty())
        return false;

    selectedRenderers.clear();
    selectedRenderers.reserve(selectedGameObjects.size());

    for (const std::shared_ptr<GameObject>& go : selectedGameObjects)
    {
        if (go && go->GetRenderer())
            selectedRenderers.push_back(go->GetRenderer());
    }

    RenderSettings settings{};
    settings.OverrideMaterial = m_SilhouetteMaterial;

    const Matrix4x4 vpMatrix = ctx.ProjectionMatrix * ctx.ViewMatrix;
    m_SelectedObjectsQueue.Prepare(vpMatrix, selectedRenderers, settings);

    return true;
}

void SelectionOutlinePass::Execute(const Context& ctx)
{
    static std::shared_ptr<Texture2D> silhouetteRenderTarget = nullptr;
    static Vector4 outlineColor {1, 0.73f, 0, 1};

    Profiler::Marker marker("SelectionOutlinePass::Execute");

    CheckTexture(silhouetteRenderTarget);

    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    // render selected gameObjects
    {
        Profiler::GPUMarker gpuMarker("Selection Outline Pass");

        GraphicsBackendRenderTargetDescriptor colorTarget{FramebufferAttachment::COLOR_ATTACHMENT0, silhouetteRenderTarget->GetBackendTexture(), LoadAction::CLEAR};
        GraphicsBackend::Current()->AttachRenderTarget(colorTarget);

        GraphicsBackend::Current()->BeginRenderPass("Selection Outline Pass");
        m_SelectedObjectsQueue.Draw();
        GraphicsBackend::Current()->EndRenderPass();
    }

    // blit to screen
    {
        struct OutlineData
        {
            Vector4 Color;

            Vector2 InvTextureSize;
            Vector2 Padding0;
        };

        static std::shared_ptr<Shader> blitShader = Shader::Load("core_resources/shaders/outlineBlit", {}, {}, {}, {false, ComparisonFunction::ALWAYS});
        static std::shared_ptr<GraphicsBuffer> blitDataBuffer = std::make_shared<GraphicsBuffer>(sizeof(OutlineData), "Selection Outline Data");

        Profiler::GPUMarker gpuMarker("Selection Blit Pass");

        OutlineData data{};
        data.Color = outlineColor;
        data.InvTextureSize = Vector2(1.0f / silhouetteRenderTarget->GetWidth(), 1.0f / silhouetteRenderTarget->GetHeight());

        const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();

        GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());

        GraphicsBackend::Current()->BeginRenderPass("Selection Blit Pass");
        blitDataBuffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(blitDataBuffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(silhouetteRenderTarget->GetBackendTexture(), silhouetteRenderTarget->GetBackendSampler(), 0);
        GraphicsBackend::Current()->UseProgram(blitShader->GetProgram(fullscreenMesh->GetVertexAttributes(), fullscreenMesh->GetPrimitiveType()));
        GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), IndicesDataType::UNSIGNED_INT);
        GraphicsBackend::Current()->EndRenderPass();
    }
}

#endif
