#include "render_pass.h"

RenderPass::RenderPass(int priority) :
    m_Priority(priority)
{
}

bool RenderPass::Comparer::operator()(const RenderPass& renderPassA, const RenderPass& renderPassB) const
{
    return renderPassA.m_Priority < renderPassB.m_Priority;
}
