#ifdef ENABLE_IMGUI

#include "graphics_settings_window.h"
#include "graphics/graphics_settings.h"
#include "global_constants.h"
#include "imgui.h"

#include <vector>
#include <functional>
#include <typeinfo>

GraphicsSettingsWindow::GraphicsSettingsWindow() : BaseWindow(500, 400, "Graphics Settings", typeid(GraphicsSettingsWindow).hash_code(), false)
{
}

void DrawFloatSetting(const std::string &name, const std::function<float()> &getter, const std::function<void(float)> &setter, float min = 0.0f, float max = 100.0f)
{
    float value = getter();
    ImGui::DragFloat(name.c_str(), &value, 0.01f, min, max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    setter(value);
}

void DrawColorSetting(const std::string &name, const std::function<Vector3()> &getter, const std::function<void(Vector3)> &setter)
{
    Vector3 value = getter();
    ImGui::ColorEdit3(name.c_str(), reinterpret_cast<float*>(&value));
    setter(value);
}

void DrawLightingSettings()
{
    ImGui::SeparatorText("Lighting");

    DrawColorSetting("Ambient Light Color", GraphicsSettings::GetAmbientLightColor, GraphicsSettings::SetAmbientLightColor);
    DrawFloatSetting("Ambient Light Intensity", GraphicsSettings::GetAmbientLightIntensity, GraphicsSettings::SetAmbientLightIntensity);

    ImGui::Spacing();

    DrawColorSetting("Sun Light Color", GraphicsSettings::GetSunLightColor, GraphicsSettings::SetSunLightColor);
    DrawFloatSetting("Sun Light Intensity", GraphicsSettings::GetSunLightIntensity, GraphicsSettings::SetSunLightIntensity);
}

void DrawTonemappingSettings()
{
    const static std::vector<std::string> tonemappingModes = {"None", "Reinhard Simple", "Reinhard Luminance", "Reinhard Luminance White Preserving", "RomBinDaHouse", "Filmic", "Uncharted 2", "ACES", "ACES Approximation"};

    ImGui::SeparatorText("Tonemapping");

    DrawFloatSetting("Gamma", GraphicsSettings::GetGamma, GraphicsSettings::SetGamma, 0.01f);
    DrawFloatSetting("Exposure", GraphicsSettings::GetExposure, GraphicsSettings::SetExposure, 0.01f);

    int selectedTonemapping = static_cast<int>(GraphicsSettings::GetTonemappingMode());
    if (ImGui::BeginCombo("Tonemapping Mode", tonemappingModes[selectedTonemapping].c_str()))
    {
        for (int i = 0; i < tonemappingModes.size(); i++)
        {
            bool isSelected = (selectedTonemapping == i);
            if (ImGui::Selectable(tonemappingModes[i].c_str(), isSelected))
            {
                selectedTonemapping = i;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    GraphicsSettings::SetTonemappingMode(static_cast<GraphicsSettings::TonemappingMode>(selectedTonemapping));
}

void DrawShadowCascadeBoundsSettings()
{
    float bounds[GlobalConstants::ShadowCascadeCount];
    for (int i = 0; i < GlobalConstants::ShadowCascadeCount; ++i)
        bounds[i] = GraphicsSettings::GetShadowCascadeBounds(i);

    ImGui::SliderFloat4("Shadow Cascade Bounds", bounds, 0.01f, 1.0f);

    bounds[GlobalConstants::ShadowCascadeCount - 1] = 1.0f;
    for (int i = 0; i < GlobalConstants::ShadowCascadeCount - 1; ++i)
        GraphicsSettings::SetShadowCascadeBounds(std::min(bounds[i], bounds[i + 1]), i);
}

void DrawShadowsSettings()
{
    ImGui::SeparatorText("Shadows");

    DrawFloatSetting("Shadow Distance", GraphicsSettings::GetShadowDistance, GraphicsSettings::SetShadowDistance, 0.1f);
    DrawShadowCascadeBoundsSettings();
}

void GraphicsSettingsWindow::DrawInternal()
{
    DrawLightingSettings();
    DrawTonemappingSettings();
    DrawShadowsSettings();
}

#endif
