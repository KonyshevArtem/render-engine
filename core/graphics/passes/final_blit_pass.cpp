#include "final_blit_pass.h"
#include "editor/profiler/profiler.h"
#include "enums/resource_state.h"
#include "graphics/render_data.h"
#include "texture_2d/texture_2d.h"
#include "material/material.h"
#include "shader/shader.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"

FinalBlitPass::FinalBlitPass() :
    RenderPass()
{
    m_BlitShader = Shader::Load("core_resources/shaders/blit", {});
}

void FinalBlitPass::Prepare(RenderData& renderData)
{
}

void FinalBlitPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("FinalBlitPass::Execute");

    const GraphicsBackendRenderTargetDescriptor colorBackbufferDesc = GraphicsBackendRenderTargetDescriptor::ColorBackbuffer();

    GraphicsBackend::Current()->AttachRenderTarget(colorBackbufferDesc);
    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    GraphicsBackend::Current()->BeginRenderPass("Final Blit Pass");
    {
        Profiler::GPUMarker gpuMarker("FinalBlitPass::Execute");

        GraphicsBackend::Current()->BindTextureSampler(renderData.PostProcessedTarget->GetBackendTexture(), renderData.PostProcessedTarget->GetBackendSampler(), 0);
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());

        const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
        GraphicsBackend::Current()->UseProgram(m_BlitShader->GetProgram(fullscreenMesh));
        GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
    }
    GraphicsBackend::Current()->EndRenderPass();
}
