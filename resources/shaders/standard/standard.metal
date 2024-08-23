#include "standard_shared.h"

Varyings vertex vertexMain( Attributes input [[stage_in]],
                            constant PerDrawDataStruct & PerDrawData [[buffer(1)]],
                            constant CameraDataStruct & CameraData [[buffer(2)]])
{
    return vertexFunction(input, PerDrawData, CameraData);
}

half4 fragment fragmentMain(Varyings input [[stage_in]],
                            constant CameraDataStruct & CameraData [[buffer (1)]],
                            constant PerMaterialDataStruct & PerMaterialData [[buffer (2)]],
                            constant LightingStruct & Lighting [[buffer (3)]],

                            texture2d<half> _Albedo [[texture (0)]],
                            sampler _AlbedoSampler [[sampler (0)]]
#ifdef _NORMAL_MAP
                            ,texture2d<half> _NormalMap [[texture (1)]]
                            ,sampler _NormalMapSampler [[sampler (1)]]
#endif
#ifdef _DATA_MAP
                            ,texture2d<half> _Data [[texture (2)]]
                            ,sampler _DataSampler [[sampler (2)]]
#endif
#ifdef _REFLECTION
                            ,texturecube<half> _ReflectionCube [[texture (3)]]
                            ,sampler _ReflectionCubeSampler [[sampler (3)]]
#endif
#ifdef _RECEIVE_SHADOWS
                            ,constant ShadowsStruct & Shadows [[buffer (4)]]
                            ,texture2d<float> _DirLightShadowMap [[texture (4)]]
                            ,sampler _DirLightShadowMapSampler [[sampler (4)]]

                            ,texture2d_array<float> _SpotLightShadowMapArray [[texture (5)]]
                            ,sampler _SpotLightShadowMapArraySampler [[sampler (5)]]

                            ,texture2d_array<float> _PointLightShadowMapArray [[texture (6)]]
                            ,sampler _PointLightShadowMapArraySampler [[sampler (6)]]
#endif
)
{
    return fragmentFunction(input, CameraData, PerMaterialData, Lighting,
#ifdef _NORMAL_MAP
        _NormalMap, _NormalMapSampler,
#endif
#ifdef _DATA_MAP
        _Data, _DataSampler,
#endif
#ifdef _REFLECTION
        _ReflectionCube, _ReflectionCubeSampler,
#endif
#ifdef _RECEIVE_SHADOWS
        Shadows,
        _DirLightShadowMap, _DirLightShadowMapSampler,
        _SpotLightShadowMapArray, _SpotLightShadowMapArraySampler,
        _PointLightShadowMapArray, _PointLightShadowMapArraySampler,
#endif
        _Albedo, _AlbedoSampler);
}