#include "skybox_pass.h"
#include "graphics/render_data.h"
#include "cubemap/cubemap.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug_group.h"
#include "graphics_buffer/graphics_buffer.h"
#include "shader/shader.h"
#include "resources/resources.h"
#include "types/graphics_backend_buffer_descriptor.h"

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

void SkyboxPass::Execute(const RenderData& renderData)
{
    struct SkyboxData
    {
        Matrix4x4 MVPMatrix;
    };

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = sizeof(SkyboxData);

    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/skybox", {});
    static const std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "Skybox Data");

    if (m_Mesh == nullptr || renderData.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebugGroup("Skybox pass", GPUQueue::RENDER);

    const Matrix4x4 modelMatrix = Matrix4x4::Translation(renderData.ViewMatrix.Invert().GetPosition());
    const Matrix4x4 mvpMatrix = renderData.ProjectionMatrix * renderData.ViewMatrix * modelMatrix;

    SkyboxData data{};
    data.MVPMatrix = mvpMatrix;

    buffer->SetData(&data, 0, sizeof(data));
    GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));

    GraphicsBackend::Current()->BindTextureSampler(renderData.Skybox->GetBackendTexture(), renderData.Skybox->GetBackendSampler(), 0);

    GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::CullFront());
    GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_Mesh));
    GraphicsBackend::Current()->DrawElements(m_Mesh->GetGraphicsBackendGeometry(), m_Mesh->GetPrimitiveType(), m_Mesh->GetElementsCount(), m_Mesh->GetIndicesDataType());
}
