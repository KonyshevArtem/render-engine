#include "render_pass.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"

#include <utility>

RenderPass::RenderPass(std::string _name, DrawCallSortMode _sorting, DrawCallFilter _filter, ClearMask _clearMask, const std::string &_lightModeTag) :
    m_Name(std::move(_name)),
    m_ClearMask(_clearMask),
    m_RenderSettings(RenderSettings {{{"LightMode", _lightModeTag}}, _sorting, _filter})
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = GraphicsBackendDebug::DebugGroup("Render pass " + m_Name);

    GraphicsBackend::SetDepthWrite(true);
    GraphicsBackend::Clear(m_ClearMask);

    Graphics::DrawRenderers(_ctx.Renderers, m_RenderSettings);
}
