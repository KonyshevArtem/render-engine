#ifndef RENDER_ENGINE_GLOBAL_CONSTANTS_H
#define RENDER_ENGINE_GLOBAL_CONSTANTS_H

// Keep in-sync with shaders/common/global_defines.h
namespace GlobalConstants
{
    constexpr const char *PerMaterialDataBufferName = "PerMaterialData";

    constexpr int ShadowCascadeCount = 4;
    constexpr int MaxSpotLightSources = 3;
    constexpr int MaxPointLightSources = 3;

    constexpr int TransparentRenderQueue = 3000;

    constexpr int ReflectionCubeIndex = 4;
    constexpr int DirectionalShadowMapIndex = 5;
    constexpr int SpotLightShadowMapIndex = 6;
    constexpr int PointLightShadowMapIndex = 7;

    constexpr int MatricesData = 4;
    constexpr int LightingDataIndex = 5;
    constexpr int CameraDataIndex = 6;
    constexpr int ShadowDataIndex = 7;

    constexpr int InstancingMatricesData = 7;
}

#endif //RENDER_ENGINE_GLOBAL_CONSTANTS_H
