#include "draw_renderers_pass.h"
#include "graphics/render_data.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"
#include "vector3/vector3.h"
#include "editor/profiler/profiler.h"

#include <utility>

DrawRenderersPass::DrawRenderersPass(std::string name, DrawCallSortMode sorting, DrawCallFilter filter) :
    RenderPass(),
    m_Name(std::move(name)),
    m_RenderSettings(RenderSettings {sorting, std::move(filter), nullptr})
{
}

void DrawRenderersPass::Prepare(RenderData& renderData)
{
    Profiler::Marker marker("DrawRenderersPass::Prepare");

    m_RenderQueue.Prepare(renderData.ProjectionMatrix * renderData.ViewMatrix, renderData.Renderers, m_RenderSettings);
}

void DrawRenderersPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("DrawRenderersPass::Execute");

    auto debugGroup = GraphicsBackendDebugGroup(m_Name, GPUQueue::RENDER);
    m_RenderQueue.Draw();
}
