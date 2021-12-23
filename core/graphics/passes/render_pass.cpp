#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "render_pass.h"
#include "../../renderer/renderer.h"
#include "../context.h"
#include "../graphics.h"
#include <OpenGL/gl3.h>

RenderPass::RenderPass()
{
}

void RenderPass::Execute(const Context &_ctx)
{
    glViewport(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Graphics::SetCameraData(_ctx.ViewMatrix, _ctx.ProjectionMatrix);

    for (const auto *r: _ctx.Renderers)
    {
        if (r != nullptr)
            r->Render();
    }
}
