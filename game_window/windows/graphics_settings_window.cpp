#include "graphics_settings_window.h"
#include "graphics/graphics_settings.h"
#include "imgui.h"

#include <vector>
#include <typeinfo>

GraphicsSettingsWindow::GraphicsSettingsWindow() : BaseWindow(500, 400, "Graphics Settings", typeid(GraphicsSettingsWindow).hash_code(), false)
{
}

void DrawLightingSettings()
{
    ImGui::SeparatorText("Lighting");

    Vector3 ambientColor = GraphicsSettings::GetAmbientLightColor();
    ImGui::ColorEdit3("Ambient Light Color", reinterpret_cast<float*>(&ambientColor));
    GraphicsSettings::SetAmbientLightColor(ambientColor);

    float ambientIntensity = GraphicsSettings::GetAmbientLightIntensity();
    ImGui::DragFloat("Ambient Light Intensity", &ambientIntensity, 0.01f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    GraphicsSettings::SetAmbientLightIntensity(ambientIntensity);

    ImGui::Spacing();

    Vector3 sunColor = GraphicsSettings::GetSunLightColor();
    ImGui::ColorEdit3("Sun Light Color", reinterpret_cast<float*>(&sunColor));
    GraphicsSettings::SetSunLightColor(sunColor);

    float sunIntensity = GraphicsSettings::GetSunLightIntensity();
    ImGui::DragFloat("Sun Light Intensity", &sunIntensity, 0.01f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    GraphicsSettings::SetSunLightIntensity(sunIntensity);
}

void DrawTonemappingSettings()
{
    static std::vector<std::string> tonemappingModes = {"No Tonemapping", "Reinhard Tonemapping"};

    ImGui::SeparatorText("Tonemapping");

    float gamma = GraphicsSettings::GetGamma();
    ImGui::DragFloat("Gamma", &gamma, 0.01f, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    GraphicsSettings::SetGamma(gamma);

    int selectedTonemapping = static_cast<int>(GraphicsSettings::GetTonemappingMode());
    if (ImGui::BeginCombo("Mode", tonemappingModes[selectedTonemapping].c_str()))
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

void GraphicsSettingsWindow::DrawInternal()
{
    DrawLightingSettings();
    DrawTonemappingSettings();
}


