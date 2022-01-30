#include "render_pass.h"
#include "../../../math/matrix4x4/matrix4x4.h"
#include "../../../math/vector3/vector3.h"
#include "../context.h"
#include "../graphics.h"
#include <algorithm>
#include <vector>
#include <iterator>

RenderPass::RenderPass(Renderer::Sorting _rendererSorting, Renderer::Filter _filter, GLbitfield _clearFlags) :
    m_Sorting(_rendererSorting), m_Filter(_filter), m_ClearFlags(_clearFlags)
{
}

void RenderPass::Execute(const Context &_ctx)
{
    glViewport(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight());
    glClear(m_ClearFlags);

    Graphics::SetCameraData(_ctx.ViewMatrix, _ctx.ProjectionMatrix);

    vector<Renderer *> filteredRenderers;
    copy_if(_ctx.Renderers.begin(), _ctx.Renderers.end(), back_inserter(filteredRenderers), m_Filter);

    Vector3 cameraPosWS = _ctx.ViewMatrix.Invert().GetPosition();
    sort(filteredRenderers.begin(), filteredRenderers.end(), Renderer::Comparer {m_Sorting, cameraPosWS});

    for (const auto *r: filteredRenderers)
    {
        if (r != nullptr)
            r->Render();
    }
}
