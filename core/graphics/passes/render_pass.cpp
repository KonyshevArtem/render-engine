#include "render_pass.h"
#include "../context.h"
#include "core_debug/debug.h"
#include "graphics/render_settings.h"
#include "matrix4x4/matrix4x4.h"
#include "vector3/vector3.h"
#include <algorithm>
#include <iterator>
#include <vector>

RenderPass::RenderPass(const std::string &_name, Renderer::Sorting _rendererSorting, Renderer::Filter _filter, GLbitfield _clearFlags, RenderSettings _renderSettings) :
    m_Name(_name),
    m_Sorting(std::move(_rendererSorting)),
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

    std::vector<Renderer *> filteredRenderers;
    copy_if(_ctx.Renderers.begin(), _ctx.Renderers.end(), std::back_inserter(filteredRenderers), m_Filter);

    Vector3 cameraPosWS = _ctx.ViewMatrix.Invert().GetPosition();
    std::sort(filteredRenderers.begin(), filteredRenderers.end(), Renderer::Comparer {m_Sorting, cameraPosWS});

    for (const auto *r: filteredRenderers)
    {
        if (r != nullptr)
            r->Render(m_RenderSettings);
    }
}
