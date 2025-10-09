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
}

#endif //RENDER_ENGINE_GLOBAL_CONSTANTS_H
