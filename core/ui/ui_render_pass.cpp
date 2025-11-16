#include "ui_render_pass.h"
#include "shader/shader.h"
#include "ui/ui_manager.h"
#include "ui/ui_image.h"
#include "ui/ui_text.h"
#include "graphics_buffer/ring_buffer.h"
#include "vector4/vector4.h"
#include "graphics/graphics.h"
#include "graphics_backend_api.h"
#include "texture_2d/texture_2d.h"
#include "texture_2d_array/texture_2d_array.h"
#include "mesh/mesh.h"
#include "editor/gizmos/gizmos.h"
#include "editor/profiler/profiler.h"

namespace UIRenderPass_Local
{
    Vector4 GetOffsetScale(Vector2 position, Vector2 size)
    {
        const Vector2& referenceSize = UIManager::GetReferenceSize();
        return Vector4(position.x / referenceSize.x, position.y / referenceSize.y, size.x / referenceSize.x, size.y / referenceSize.y);
    }
}

UIRenderPass::UIRenderPass(int priority) :
    RenderPass(priority)
{
    m_ImageShader = Shader::Load("core_resources/shaders/ui/image", {}, {true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA}, {CullFace::NONE}, {false, ComparisonFunction::ALWAYS});
    m_TextShader = Shader::Load("core_resources/shaders/ui/text", {}, {true, BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA}, {CullFace::NONE}, {false, ComparisonFunction::ALWAYS});
    m_UIDataBuffer = std::make_shared<RingBuffer>(1024, "UI Data");
}

void UIRenderPass::Prepare(const Context& ctx)
{
    Profiler::Marker _("UIRenderPass::Prepare");

    for (UIElement* element : UIManager::GetElements())
    {
        if (UIText* text = dynamic_cast<UIText*>(element))
            text->PrepareFont();
    }

    const int width = Graphics::GetScreenWidth();
    const int height = Graphics::GetScreenHeight();
    for (UIElement* element : UIManager::GetElements())
    {
        if (UIText* text = dynamic_cast<UIText*>(element))
            text->PrepareMesh();

        Vector4 offsetScale = UIRenderPass_Local::GetOffsetScale(element->GetGlobalPosition(), element->Size);
        Vector2 position = Vector2(offsetScale.x * width, offsetScale.y * height);
        Vector2 size = Vector2(offsetScale.z * width, offsetScale.w * height);
        Gizmos::DrawRect(position, position + size);
    }
}

void UIRenderPass::Execute(const Context& ctx)
{
    Profiler::Marker cpuMarker("UIRenderPass::Execute");
    Profiler::GPUMarker gpuMarker("UIRenderPass::Execute");

    struct UIData
    {
        Vector4 OffsetScale;
        Vector4 Color;
    };

    const std::shared_ptr<Mesh> quadMesh = Mesh::GetQuadMesh();

    GraphicsBackend::Current()->BeginRenderPass("UI Pass");

    for (UIElement* element : UIManager::GetElements())
    {
        if (UIImage* image = dynamic_cast<UIImage*>(element))
        {
            UIData data;
            data.OffsetScale = UIRenderPass_Local::GetOffsetScale(image->GetGlobalPosition(), image->Size);
            data.Color = image->Color;
            uint64_t offset = m_UIDataBuffer->SetData(&data, 0, sizeof(data));

            GraphicsBackend::Current()->BindConstantBuffer(m_UIDataBuffer->GetBackendBuffer(), 0, offset, sizeof(data));
            GraphicsBackend::Current()->BindTextureSampler(image->Image->GetBackendTexture(), image->Image->GetBackendSampler(), 0);

            GraphicsBackend::Current()->UseProgram(m_ImageShader->GetProgram(quadMesh));
            GraphicsBackend::Current()->DrawElements(quadMesh->GetGraphicsBackendGeometry(), quadMesh->GetPrimitiveType(), quadMesh->GetElementsCount(), quadMesh->GetIndicesDataType());
        }

        if (UIText* text = dynamic_cast<UIText*>(element))
        {
            UIData data;
            data.OffsetScale = UIRenderPass_Local::GetOffsetScale(text->GetGlobalPosition(), Vector2(1, 1));
            data.Color = Vector4(1, 1, 1, 1);
            uint64_t offset = m_UIDataBuffer->SetData(&data, 0, sizeof(data));

            GraphicsBackend::Current()->BindConstantBuffer(m_UIDataBuffer->GetBackendBuffer(), 0, offset, sizeof(data));
            GraphicsBackend::Current()->BindTextureSampler(text->GetFontAtlas()->GetBackendTexture(), text->GetFontAtlas()->GetBackendSampler(), 0);

            const std::shared_ptr<Mesh> textMesh = text->GetMesh();
            GraphicsBackend::Current()->UseProgram(m_TextShader->GetProgram(textMesh));
            GraphicsBackend::Current()->DrawElements(textMesh->GetGraphicsBackendGeometry(), textMesh->GetPrimitiveType(), textMesh->GetElementsCount(), textMesh->GetIndicesDataType());
        }
    }

    GraphicsBackend::Current()->EndRenderPass();
}
