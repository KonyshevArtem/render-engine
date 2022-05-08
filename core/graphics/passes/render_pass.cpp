#include "render_pass.h"
#include "../context.h"
#include "core_debug/debug.h"
#include "graphics/graphics.h"
#include <algorithm>
#include <iterator>
#include <vector>

RenderPass::RenderPass(const std::string &_name, DrawCallInfo::Sorting _sorting, DrawCallInfo::Filter _filter, GLbitfield _clearFlags, RenderSettings _renderSettings) :
    m_Name(_name),
    m_Sorting(std::move(_sorting)),
    m_Filter(std::move(_filter)),
    m_ClearFlags(_clearFlags),
    m_RenderSettings(std::move(_renderSettings))
{
}

void RenderPass::Execute(const Context &_ctx)
{
    auto debugGroup = Debug::DebugGroup("Render pass " + m_Name);

    glDepthMask(GL_TRUE);
    glClear(m_ClearFlags);

    std::vector<DrawCallInfo> filteredInfos;
    copy_if(_ctx.DrawCallInfos.begin(), _ctx.DrawCallInfos.end(), std::back_inserter(filteredInfos), m_Filter);

    Vector3 cameraPosWS = _ctx.ViewMatrix.Invert().GetPosition();
    std::sort(filteredInfos.begin(), filteredInfos.end(), DrawCallInfo::Comparer {m_Sorting, cameraPosWS});

    Graphics::Draw(_ctx.DrawCallInfos, m_RenderSettings);
}
