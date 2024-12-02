#include "draw_renderers_pass.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"

#include <utility>

DrawRenderersPass::DrawRenderersPass(std::string name, DrawCallSortMode sorting, DrawCallFilter filter, int priority) :
    RenderPass(priority),
    m_Name(std::move(name)),
    m_RenderSettings(RenderSettings {sorting, std::move(filter), nullptr})
{
}

void DrawRenderersPass::Prepare()
{
}

void DrawRenderersPass::Execute(const Context& ctx)
{
    auto debugGroup = GraphicsBackendDebugGroup(m_Name);

    Graphics::DrawRenderers(ctx.Renderers, m_RenderSettings);
}
