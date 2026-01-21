#include "skybox_pass.h"
#include "graphics/context.h"
#include "cubemap/cubemap.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug_group.h"
#include "graphics_buffer/graphics_buffer.h"
#include "shader/shader.h"
#include "resources/resources.h"

std::shared_ptr<Mesh> SkyboxPass::m_Mesh = nullptr;

SkyboxPass::SkyboxPass(int priority) :
    RenderPass(priority)
{
}

void SkyboxPass::Prepare()
{
    if (!m_Mesh)
        m_Mesh = Resources::Load<Mesh>("core_resources/models/Cube");
}

void SkyboxPass::Execute(const Context& ctx)
{
    struct SkyboxData
    {
        Matrix4x4 MVPMatrix;
    };

    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/skybox", {}, {}, {CullFace::FRONT}, {});
    static const std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(sizeof(SkyboxData), "Skybox Data");

    if (m_Mesh == nullptr || ctx.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebugGroup("Skybox pass", GPUQueue::RENDER);

    const Matrix4x4 modelMatrix = Matrix4x4::Translation(ctx.ViewMatrix.Invert().GetPosition());
    const Matrix4x4 mvpMatrix = ctx.ProjectionMatrix * ctx.ViewMatrix * modelMatrix;

    SkyboxData data{};
    data.MVPMatrix = mvpMatrix;

    buffer->SetData(&data, 0, sizeof(data));
    GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));

    GraphicsBackend::Current()->BindTextureSampler(ctx.Skybox->GetBackendTexture(), ctx.Skybox->GetBackendSampler(), 0);

    GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_Mesh));
    GraphicsBackend::Current()->DrawElements(m_Mesh->GetGraphicsBackendGeometry(), m_Mesh->GetPrimitiveType(), m_Mesh->GetElementsCount(), m_Mesh->GetIndicesDataType());
}
