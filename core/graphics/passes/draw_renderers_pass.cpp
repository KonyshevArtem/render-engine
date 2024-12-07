#include "draw_renderers_pass.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"
#include "vector3/vector3.h"
#include "editor/profiler/profiler.h"

#include <utility>

DrawRenderersPass::DrawRenderersPass(std::string name, DrawCallSortMode sorting, DrawCallFilter filter, int priority) :
    RenderPass(priority),
    m_Name(std::move(name)),
    m_RenderSettings(RenderSettings {sorting, std::move(filter), nullptr})
{
}

void DrawRenderersPass::Prepare(const Vector3& cameraPosition, const std::vector<std::shared_ptr<Renderer>>& renderers)
{
    Profiler::Marker marker("DrawRenderersPass::Prepare");

    m_RenderQueue.Prepare(cameraPosition, renderers, m_RenderSettings);
}

void DrawRenderersPass::Execute(const Context& ctx)
{
    Profiler::Marker marker("DrawRenderersPass::Execute");

    auto debugGroup = GraphicsBackendDebugGroup(m_Name);
    Graphics::DrawRenderQueue(m_RenderQueue);
}
