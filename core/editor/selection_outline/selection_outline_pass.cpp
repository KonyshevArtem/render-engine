#if RENDER_ENGINE_EDITOR

#include "selection_outline_pass.h"
#include "material/material.h"
#include "shader/shader.h"
#include "editor/hierarchy.h"
#include "texture_2d/texture_2d.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug_group.h"
#include "renderer/renderer.h"
#include "gameObject/gameObject.h"
#include "enums/framebuffer_attachment.h"
#include "enums/load_action.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "editor/profiler/profiler.h"
#include "vector2/vector2.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"
#include "graphics/render_data.h"
#include "graphics/render_settings/render_settings.h"
#include "types/graphics_backend_buffer_descriptor.h"

void CheckTexture(std::shared_ptr<Texture2D>& texture, int width, int height)
{
    if (!texture || texture->GetWidth() != width || texture->GetHeight() != height)
    {
        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = width;
        descriptor.Height = height;
        descriptor.RenderTarget = true;
        descriptor.Format = TextureInternalFormat::RGBA8;

        texture = Texture2D::Create(descriptor, "SilhouetteRT");
        texture->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
    }
}

SelectionOutlinePass::SelectionOutlinePass(int priority) :
    RenderPass(priority)
{
    m_SilhouetteMaterial = std::make_shared<Material>(Shader::Load("core_resources/shaders/silhouette", {}), "Silhouette");
    m_SilhouetteMaterial->DepthDescriptor = GraphicsBackendDepthDescriptor::AlwaysPassNoWrite();
}

void SelectionOutlinePass::Prepare(RenderData& renderData)
{
    static std::vector<std::shared_ptr<Renderer>> selectedRenderers;

    Profiler::Marker marker("SelectionOutlinePass::Prepare");

    const std::unordered_set<std::shared_ptr<GameObject>>& selectedGameObjects = Hierarchy::GetSelectedGameObjects();
    if (selectedGameObjects.empty())
        return;

    selectedRenderers.clear();
    selectedRenderers.reserve(selectedGameObjects.size());

    for (const std::shared_ptr<GameObject>& go : selectedGameObjects)
    {
        if (go && go->GetRenderer())
            selectedRenderers.push_back(go->GetRenderer());
    }

    RenderSettings settings{};
    settings.OverrideMaterial = m_SilhouetteMaterial;

    const Matrix4x4 vpMatrix = renderData.ProjectionMatrix * renderData.ViewMatrix;
    m_SelectedObjectsQueue.Prepare(vpMatrix, selectedRenderers, settings);
}

void SelectionOutlinePass::Execute(const RenderData& renderData)
{
    static std::shared_ptr<Texture2D> silhouetteRenderTarget = nullptr;
    static Vector4 outlineColor {1, 0.73f, 0, 1};

    Profiler::Marker marker("SelectionOutlinePass::Execute");

    if (m_SelectedObjectsQueue.IsEmpty())
        return;

    CheckTexture(silhouetteRenderTarget, renderData.Viewport.x, renderData.Viewport.y);

    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    // render selected gameObjects
    {
        Profiler::GPUMarker gpuMarker("Selection Outline Pass");

        const GraphicsBackendRenderTargetDescriptor colorTarget{ FramebufferAttachment::COLOR_ATTACHMENT0, silhouetteRenderTarget->GetBackendTexture(), LoadAction::CLEAR, StoreAction::STORE };
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

        GraphicsBackendBufferDescriptor bufferDescriptor{};
        bufferDescriptor.AllowCPUWrites = true;
        bufferDescriptor.Size = sizeof(OutlineData);

        static std::shared_ptr<Shader> blitShader = Shader::Load("core_resources/shaders/outlineBlit", {});
        static std::shared_ptr<GraphicsBuffer> blitDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "Selection Outline Data");

        Profiler::GPUMarker gpuMarker("Selection Blit Pass");

        OutlineData data{};
        data.Color = outlineColor;
        data.InvTextureSize = Vector2(1.0f / silhouetteRenderTarget->GetWidth(), 1.0f / silhouetteRenderTarget->GetHeight());

        const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();

        const GraphicsBackendRenderTargetDescriptor colorTarget{ FramebufferAttachment::COLOR_ATTACHMENT0, renderData.CameraColorTarget->GetBackendTexture(), LoadAction::LOAD, StoreAction::STORE };
        GraphicsBackend::Current()->AttachRenderTarget(colorTarget);

        GraphicsBackend::Current()->BeginRenderPass("Selection Blit Pass");

        blitDataBuffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(blitDataBuffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(silhouetteRenderTarget->GetBackendTexture(), silhouetteRenderTarget->GetBackendSampler(), 0);

        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::AlwaysPassNoWrite());

        GraphicsBackend::Current()->UseProgram(blitShader->GetProgram(fullscreenMesh));
        GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());

        GraphicsBackend::Current()->EndRenderPass();
    }
}

#endif
