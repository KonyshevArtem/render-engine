#include "gizmos_pass.h"
#include "core_debug/debug.h"
#include "graphics/context.h"
#include "shader/shader.h"
#include "gizmos.h"

void GizmosPass::Execute(Context &_context)
{
    static std::shared_ptr<Shader> gizmosShader = Shader::Load("resources/shaders/gizmos/gizmos.shader", {});

    Debug::PushDebugGroup("Gizmos pass");

    gizmosShader->Use();

    for (const auto &drawInfo: Gizmos::GetDrawInfos())
    {
        gizmosShader->SetUniform("_ModelMatrix", &drawInfo.Matrix);
        drawInfo.Primitive->Draw();
    }

    Shader::DetachCurrentShader();

    Debug::PopDebugGroup();
}
