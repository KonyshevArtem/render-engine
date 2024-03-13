#include "graphics_settings.h"

Vector3 s_AmbientLightColor = {1, 1, 1};
float s_AmbientLightIntensity = 0.1f;

Vector3 s_SunLightColor = {1, 1, 1};
float s_SunLightIntensity = 5;

float s_Gamma = 2.2f;
GraphicsSettings::TonemappingMode s_TonemappingMode = GraphicsSettings::TonemappingMode::REINHARD;

void GraphicsSettings::SetAmbientLightColor(const Vector3 &color)
{
    s_AmbientLightColor = color;
}

void GraphicsSettings::SetAmbientLightIntensity(float intensity)
{
    s_AmbientLightIntensity = intensity;
}

Vector3 GraphicsSettings::GetAmbientLightColor()
{
    return s_AmbientLightColor;
}

float GraphicsSettings::GetAmbientLightIntensity()
{
    return s_AmbientLightIntensity;
}

void GraphicsSettings::SetSunLightColor(const Vector3 &color)
{
    s_SunLightColor = color;
}

void GraphicsSettings::SetSunLightIntensity(float intensity)
{
    s_SunLightIntensity = intensity;
}

Vector3 GraphicsSettings::GetSunLightColor()
{
    return s_SunLightColor;
}

float GraphicsSettings::GetSunLightIntensity()
{
    return s_SunLightIntensity;
}

void GraphicsSettings::SetGamma(float gamma)
{
    s_Gamma = gamma;
}

float GraphicsSettings::GetGamma()
{
    return s_Gamma;
}

void GraphicsSettings::SetTonemappingMode(TonemappingMode mode)
{
    s_TonemappingMode = mode;
}

GraphicsSettings::TonemappingMode GraphicsSettings::GetTonemappingMode()
{
    return s_TonemappingMode;
}