#include "render_pass.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"

#include <utility>

RenderPass::RenderPass(std::string _name, DrawCallSortMode _sorting, DrawCallFilter _filter, const std::string &_lightModeTag) :
    m_Name(std::move(_name)),
    m_RenderSettings(RenderSettings {{{"LightMode", _lightModeTag}}, _sorting, std::move(_filter)})
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = GraphicsBackendDebug::DebugGroup("Render pass " + m_Name);

    Graphics::DrawRenderers(_ctx.Renderers, m_RenderSettings);
}
