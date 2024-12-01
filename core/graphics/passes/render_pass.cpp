#include "render_pass.h"

RenderPass::RenderPass(int priority) :
    m_Priority(priority)
{
}

bool RenderPass::Comparer::operator()(const std::shared_ptr<RenderPass>& renderPassA, const std::shared_ptr<RenderPass>& renderPassB) const
{
    if (!renderPassA)
        return false;
    if (!renderPassB)
        return true;
    return renderPassA->m_Priority < renderPassB->m_Priority;
}
