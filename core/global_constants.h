#ifndef RENDER_ENGINE_GLOBAL_CONSTANTS_H
#define RENDER_ENGINE_GLOBAL_CONSTANTS_H

#include <string>

namespace GlobalConstants
{
    constexpr const char *ShadowsBufferName = "Shadows";
    constexpr const char *LightingBufferName = "Lighting";
    constexpr const char *CameraDataBufferName = "CameraData";
    constexpr const char *PerDrawDataBufferName = "PerDrawData";
    constexpr const char *PerInstanceStructName = "PerInstance";
    constexpr const char *PerMaterialDataBufferName = "PerMaterialData";
    constexpr const char *PerInstanceDataBufferName = "PerInstanceData";
    constexpr const char *InstanceMatricesBufferName = "InstanceMatricesBuffer";

    constexpr int MaxInstancingCount = 256;
    constexpr int InstancingBaseVertexAttribute = 4;

    constexpr int MaxSpotLightSources = 3;
    constexpr int MaxPointLightSources = 3;

    constexpr int TransparentRenderQueue = 3000;
}

#endif //RENDER_ENGINE_GLOBAL_CONSTANTS_H
