#include "render_pass.h"
#include "graphics/context.h"
#include "debug.h"
#include "graphics/graphics.h"

#include <utility>

RenderPass::RenderPass(std::string _name, DrawCallInfo::Sorting _sorting, DrawCallInfo::Filter _filter, ClearMask _clearMask, const std::string &_lightModeTag) :
    m_Name(std::move(_name)),
    m_ClearMask(_clearMask),
    m_RenderSettings(RenderSettings {{{"LightMode", _lightModeTag}}, _sorting, _filter})
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = Debug::DebugGroup("Render pass " + m_Name);

    GraphicsBackend::SetDepthWrite(true);
    GraphicsBackend::Clear(m_ClearMask);

    Graphics::Draw(_ctx.DrawCallInfos, m_RenderSettings);
}
