#include "final_blit_pass.h"
#include "texture_2d/texture_2d.h"
#include "material/material.h"
#include "shader/shader.h"
#include "graphics/graphics.h"
#include "graphics/graphics_settings.h"
#include "types/graphics_backend_render_target_descriptor.h"

FinalBlitPass::FinalBlitPass(int priority) :
    RenderPass(priority),
    m_Source(nullptr)
{
}

void FinalBlitPass::Prepare(const std::shared_ptr<Texture2D>& source)
{
    m_Source = source;
}

void FinalBlitPass::Execute(const Context& ctx)
{
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/final_blit", {}, {}, {}, {false, DepthFunction::ALWAYS});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "FinalBlit");

    const GraphicsSettings::TonemappingMode tonemappingMode = GraphicsSettings::GetTonemappingMode();

    material->SetFloat("_OneOverGamma", 1 / GraphicsSettings::GetGamma());
    material->SetFloat("_Exposure", GraphicsSettings::GetExposure());
    material->SetInt("_TonemappingMode", static_cast<int>(tonemappingMode));
    Graphics::Blit(m_Source, nullptr, GraphicsBackendRenderTargetDescriptor::ColorBackbuffer(), *material, "Final Blit Pass");
}
