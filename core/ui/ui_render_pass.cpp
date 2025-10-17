#include "ui_render_pass.h"
#include "shader/shader.h"
#include "ui/ui_manager.h"
#include "ui/ui_image.h"
#include "graphics_buffer/ring_buffer.h"
#include "vector4/vector4.h"
#include "graphics/graphics.h"
#include "graphics_backend_api.h"
#include "texture_2d/texture_2d.h"
#include "mesh/mesh.h"

UIRenderPass::UIRenderPass(int priority) :
    RenderPass(priority)
{
    m_UIShader = Shader::Load("core_resources/shaders/ui/image", {}, {true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA}, {CullFace::NONE}, {false, ComparisonFunction::ALWAYS});
    m_UIDataBuffer = std::make_shared<RingBuffer>(1024, "UI Data");
}

void UIRenderPass::Execute(const Context& ctx)
{
    struct UIData
    {
        Vector4 OffsetScale;
        Vector4 Color;
    };

    const int width = Graphics::GetScreenWidth();
    const int height = Graphics::GetScreenHeight();
    const std::shared_ptr<Mesh> mesh = Mesh::GetQuadMesh();

    GraphicsBackend::Current()->BeginRenderPass("UI Pass");

    for (const std::shared_ptr<UIElement>& element : UIManager::Elements)
    {
        std::shared_ptr<UIImage> image = std::dynamic_pointer_cast<UIImage>(element);
        if (image)
        {
            UIData data;
            data.OffsetScale = Vector4(image->Position.x / width, image->Position.y / height, image->Size.x / width, image->Size.y / height);
            data.Color = image->Color;
            uint64_t offset = m_UIDataBuffer->SetData(&data, 0, sizeof(data));

            GraphicsBackend::Current()->BindConstantBuffer(m_UIDataBuffer->GetBackendBuffer(), 0, offset, sizeof(data));
            GraphicsBackend::Current()->BindTextureSampler(image->Image->GetBackendTexture(), image->Image->GetBackendSampler(), 0);

            GraphicsBackend::Current()->UseProgram(m_UIShader->GetProgram(mesh));
            GraphicsBackend::Current()->DrawElements(mesh->GetGraphicsBackendGeometry(), mesh->GetPrimitiveType(), mesh->GetElementsCount(), mesh->GetIndicesDataType());
        }
    }

    GraphicsBackend::Current()->EndRenderPass();
}
