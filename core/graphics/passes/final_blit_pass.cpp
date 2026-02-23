#include "final_blit_pass.h"
#include "editor/profiler/profiler.h"
#include "texture_2d/texture_2d.h"
#include "material/material.h"
#include "shader/shader.h"
#include "graphics/graphics.h"
#include "graphics/graphics_settings.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"

FinalBlitPass::FinalBlitPass(int priority) :
    RenderPass(priority),
    m_Source(nullptr)
{
}

void FinalBlitPass::Prepare(const std::shared_ptr<Texture2D>& source)
{
    m_Source = source;
}

void FinalBlitPass::Execute(const Context& ctx)
{
    struct BlitData
    {
        float OneOverGamma;
        float Exposure;
        uint32_t TonemappingMode;
        float Padding0;
    };

    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/final_blit", {});
    static const std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(sizeof(BlitData), "Final Blit Data");

    Profiler::Marker marker("FinalBlitPass::Execute");
    Profiler::GPUMarker gpuMarker("FinalBlitPass::Execute");

    BlitData data{};
    data.OneOverGamma = 1 / GraphicsSettings::GetGamma();
    data.Exposure = GraphicsSettings::GetExposure();
    data.TonemappingMode = static_cast<uint32_t>(GraphicsSettings::GetTonemappingMode());

    const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();

    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    GraphicsBackend::Current()->BeginRenderPass("Final Blit Pass");
    buffer->SetData(&data, 0, sizeof(data));
    GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));
    GraphicsBackend::Current()->BindTextureSampler(m_Source->GetBackendTexture(), m_Source->GetBackendSampler(), 0);
    GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());
    GraphicsBackend::Current()->UseProgram(shader->GetProgram(fullscreenMesh));
    GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
    GraphicsBackend::Current()->EndRenderPass();
}
