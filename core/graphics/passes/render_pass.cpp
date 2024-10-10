#include "render_pass.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"

#include <utility>

RenderPass::RenderPass(std::string _name, DrawCallSortMode _sorting, DrawCallFilter _filter, const std::string &_lightModeTag) :
    m_Name(std::move(_name)),
    m_RenderSettings(RenderSettings {_sorting, std::move(_filter), nullptr})
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = GraphicsBackendDebugGroup(m_Name);

    Graphics::DrawRenderers(_ctx.Renderers, m_RenderSettings);
}
