#include "render_pass.h"
#include "../../../math/matrix4x4/matrix4x4.h"
#include "../../../math/vector3/vector3.h"
#include "../../core_debug/debug.h"
#include "../context.h"
#include <algorithm>
#include <vector>
#include <iterator>

RenderPass::RenderPass(const std::string &_name, Renderer::Sorting _rendererSorting, Renderer::Filter _filter, GLbitfield _clearFlags) :
    m_Name(_name), m_Sorting(_rendererSorting), m_Filter(_filter), m_ClearFlags(_clearFlags)
{
}

void RenderPass::Execute(const Context &_ctx)
{
    Debug::PushDebugGroup("Render pass " + m_Name);

    glClear(m_ClearFlags);

    std::vector<Renderer *> filteredRenderers;
    copy_if(_ctx.Renderers.begin(), _ctx.Renderers.end(), std::back_inserter(filteredRenderers), m_Filter);

    Vector3 cameraPosWS = _ctx.ViewMatrix.Invert().GetPosition();
    std::sort(filteredRenderers.begin(), filteredRenderers.end(), Renderer::Comparer {m_Sorting, cameraPosWS});

    for (const auto *r: filteredRenderers)
    {
        if (r != nullptr)
            r->Render();
    }

    Debug::PopDebugGroup();
}
