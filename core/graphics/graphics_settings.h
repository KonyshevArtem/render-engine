#ifndef RENDER_ENGINE_GRAPHICS_SETTINGS_H
#define RENDER_ENGINE_GRAPHICS_SETTINGS_H

#include "vector3/vector3.h"

namespace GraphicsSettings
{
    enum class TonemappingMode
    {
        NONE        = 0,
        REINHARD    = 1,
    };

    void SetAmbientLightColor(const Vector3 &color);
    void SetAmbientLightIntensity(float intensity);
    Vector3 GetAmbientLightColor();
    float GetAmbientLightIntensity();

    void SetSunLightColor(const Vector3 &color);
    void SetSunLightIntensity(float intensity);
    Vector3 GetSunLightColor();
    float GetSunLightIntensity();

    void SetGamma(float gamma);
    float GetGamma();

    void SetTonemappingMode(TonemappingMode mode);
    TonemappingMode GetTonemappingMode();
}

#endif //RENDER_ENGINE_GRAPHICS_SETTINGS_H
