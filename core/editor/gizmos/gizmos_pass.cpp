#if RENDER_ENGINE_EDITOR

#include "gizmos_pass.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug.h"
#include "shader/shader.h"

void GizmosPass::Execute(Context &_context)
{
    static auto gizmosMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/gizmos/gizmos.shader", {}));

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Gizmos pass");

    const auto &gizmos = Gizmos::GetGizmosToDraw();
    for (const auto &pair : gizmos)
    {
        Graphics::Draw(*pair.first, *gizmosMaterial, pair.second, 0);
    }
}

#endif