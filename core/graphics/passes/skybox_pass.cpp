#include "skybox_pass.h"
#include "graphics/render_data.h"
#include "cubemap/cubemap.h"
#include "mesh/mesh.h"
#include "editor/profiler/profiler.h"
#include "graphics_buffer/graphics_buffer.h"
#include "shader/shader.h"
#include "resources/resources.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "types/graphics_backend_render_target_descriptor.h"

std::shared_ptr<Mesh> SkyboxPass::m_Mesh = nullptr;

void SkyboxPass::Prepare(RenderData& renderData)
{
	Profiler::Marker marker("SkyboxPass::Prepare");

    if (!m_Mesh)
        m_Mesh = Resources::Load<Mesh>("core_resources/models/Cube");
}

void SkyboxPass::Execute(const RenderData& renderData)
{
	Profiler::Marker marker("SkyboxPass::Execute");

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

    const Matrix4x4 modelMatrix = Matrix4x4::Translation(renderData.ViewMatrix.Invert().GetPosition());
    const Matrix4x4 mvpMatrix = renderData.ProjectionMatrix * renderData.ViewMatrix * modelMatrix;

    SkyboxData data{};
    data.MVPMatrix = mvpMatrix;
    buffer->SetData(&data, 0, sizeof(data));

	GraphicsBackendRenderTargetDescriptor colorTarget{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
	GraphicsBackendRenderTargetDescriptor depthTarget{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };

	GraphicsBackend::Current()->AttachRenderTarget(colorTarget);
	GraphicsBackend::Current()->AttachRenderTarget(depthTarget);

	GraphicsBackendDepthDescriptor depthDescriptor{};
	depthDescriptor.Enabled = true;
	depthDescriptor.WriteDepth = false;
    depthDescriptor.DepthFunction = ComparisonFunction::LEQUAL;
	GraphicsBackend::Current()->SetDepthState(depthDescriptor);

    GraphicsBackend::Current()->BeginRenderPass("Skybox Pass");
    {
		Profiler::GPUMarker gpuMarker("SkyboxPass::Execute");
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));

        GraphicsBackend::Current()->BindTextureSampler(renderData.Skybox->GetBackendTexture(), renderData.Skybox->GetBackendSampler(), 0);

        GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::CullFront());
        GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_Mesh));
        GraphicsBackend::Current()->DrawElements(m_Mesh->GetGraphicsBackendGeometry(), m_Mesh->GetPrimitiveType(), m_Mesh->GetElementsCount(), m_Mesh->GetIndicesDataType());
    }
	GraphicsBackend::Current()->EndRenderPass();
}
