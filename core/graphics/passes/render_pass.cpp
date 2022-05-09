#include "render_pass.h"
#include "../context.h"
#include "core_debug/debug.h"
#include "graphics/graphics.h"
#include <algorithm>
#include <iterator>
#include <vector>

RenderPass::RenderPass(const std::string &_name, DrawCallInfo::Sorting _sorting, DrawCallInfo::Filter _filter, GLbitfield _clearFlags) :
    m_Name(_name),
    m_ClearFlags(_clearFlags),
    m_RenderSettings(RenderSettings {{{"LightMode", "Forward"}}, _sorting, _filter})
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = Debug::DebugGroup("Render pass " + m_Name);

    CHECK_GL(glDepthMask(GL_TRUE));
    CHECK_GL(glClear(m_ClearFlags));

    Graphics::Draw(_ctx.DrawCallInfos, m_RenderSettings);
}
