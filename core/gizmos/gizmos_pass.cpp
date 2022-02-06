#include "gizmos_pass.h"
#include "../core_debug/debug.h"
#include "../graphics/context.h"
#include "../shader/shader.h"
#include "gizmos.h"

void GizmosPass::Execute(Context &_context)
{
    static std::shared_ptr<Shader> m_GizmosShader = nullptr;
    if (!m_GizmosShader)
        m_GizmosShader = Shader::Load("resources/shaders/gizmos/gizmos.shader", {});

    Debug::PushDebugGroup("Gizmos pass");

    m_GizmosShader->Use();

    for (const auto &drawInfo: Gizmos::GetDrawInfos())
    {
        m_GizmosShader->SetUniform("_ModelMatrix", &drawInfo.Matrix);
        drawInfo.Primitive->Draw();
    }

    Shader::DetachCurrentShader();

    Debug::PopDebugGroup();
}
