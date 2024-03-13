#include "final_blit_pass.h"
#include "texture_2d/texture_2d.h"
#include "material/material.h"
#include "shader/shader.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug.h"
#include "graphics/graphics_settings.h"

void FinalBlitPass::Execute(Context &context, const std::shared_ptr<Texture2D> &source)
{
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("resources/shaders/final_blit/final_blit.shader", {}));

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Final Blit");

    GraphicsSettings::TonemappingMode tonemappingMode = GraphicsSettings::GetTonemappingMode();

    material->SetFloat("_OneOverGamma", 1 / GraphicsSettings::GetGamma());
    material->SetFloat("_Exposure", GraphicsSettings::GetExposure());
    material->SetInt("_TonemappingMode", static_cast<int>(tonemappingMode));
    Graphics::Blit(source, nullptr, 0, 0, *material);
}