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

    enum TextureIndex
    {
        PROBE_LIGHT = 3,
        REFLECTION_CUBE = 4,
        DIRECTIONAL_SHADOW_MAP = 5,
        PUNCTUAL_LIGHT_SHADOW_ATLAS = 6,
        PROBE_DISTANCES = 7,

        RT_SHADOW_MASK = DIRECTIONAL_SHADOW_MAP,
    };

    enum ConstantBufferIndex
    {
        PROBE_DATA = 4,
		LIGHTING_DATA= 5,
		CAMERA_DATA = 6,
		SHADOW_DATA = 7,
    };

    enum BufferIndex
    {
        RT_PER_INSTANCE_DATA = 5,
        INSTANCING_MATRICES_ENTRIES = 6,
		TRANSFORM_MATRICES = 7,
    };

    enum TLASIndex
    {
        RT_SCENE = 0,
    };
}

#endif //RENDER_ENGINE_GLOBAL_CONSTANTS_H
