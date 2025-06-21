#ifndef RENDER_ENGINE_LIGHTING_DATA_H
#define RENDER_ENGINE_LIGHTING_DATA_H

#include "global_constants.h"
#include "vector3/vector3.h"
#include "vector4/vector4.h"

#include <cstdint>

// Keep this in-sync with shaders/common/lighting.cg
struct LightingData
{
    struct PointLightData
    {
        Vector4 Position{};
        Vector3 Intensity{};
        float Range = 0;
    };

    struct SpotLightData
    {
        Vector4 Position{};
        Vector3 Direction{};
        float Range = 0;
        Vector3 Intensity{};
        float CutOffCosine = 0;
    };

    Vector3 AmbientLight{};
    uint32_t PointLightsCount = 0;

    Vector3 DirLightDirection{};
    float HasDirectionalLight = 0;
    Vector3 DirLightIntensity{};

    uint32_t SpotLightsCount = 0;

    PointLightData PointLightsData[GlobalConstants::MaxPointLightSources]{};
    SpotLightData SpotLightsData[GlobalConstants::MaxSpotLightSources]{};
};

#endif //RENDER_ENGINE_LIGHTING_DATA_H
