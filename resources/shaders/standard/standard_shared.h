#ifndef STANDARD_SHADER_SHARED_CODE
#define STANDARD_SHADER_SHARED_CODE

#include "../common/graphics_backend_macros.h"
#include "../common/per_draw_data.h"
#include "../common/camera_data.h"
#include "../common/lighting.h"

#ifdef _PER_INSTANCE_DATA
    BEGIN_PER_INSTANCE_DATA
        float4 _Color;
        float _Size;
    END_PER_INSTANCE_DATA
#endif

struct Attributes
{
    float3 vertPositionOS   ATTRIBUTE(0);
    float3 vertNormalOS     ATTRIBUTE(1);
    float2 texCoord         ATTRIBUTE(2);
    float3 vertTangentOS    ATTRIBUTE(3);
};

struct Varyings
{
    DECLARE_CLIP_POS(float4, PositionCS)
    float3 PositionWS;
    float3 NormalWS;
#ifdef _NORMAL_MAP
    float3 TangentWS;
#endif
    float2 UV;
    DECLARE_INSTANCE_ID_VARYING
};

struct PerMaterialDataStruct
{
    float4 _Albedo_ST;
#ifndef _DATA_MAP
    float _Roughness;
    float _Metallness;
#endif
#ifdef _NORMAL_MAP
    float4 _NormalMap_ST;
    float _NormalIntensity;
#endif
#ifdef _REFLECTION
    float _ReflectionCubeLevels;
#endif
};

#endif

Varyings vertexFunction(Attributes attributes, PerDrawDataStruct perDrawData, CameraDataStruct cameraData)
{
    Varyings vars;

    TRANSFER_INSTANCE_ID_VARYING(vars)
    SETUP_INSTANCE_ID(vars)

    float3 vertPos = attributes.vertPositionOS;
#ifdef _PER_INSTANCE_DATA
    vertPos *= GET_PER_INSTANCE_VALUE(_Size);
#endif

    vars.PositionWS = (perDrawData._ModelMatrix * float4(vertPos, 1)).xyz;
    vars.NormalWS = normalize((perDrawData._ModelNormalMatrix * float4(attributes.vertNormalOS, 0)).xyz);
#ifdef _NORMAL_MAP
    vars.TangentWS = normalize((perDrawData._ModelNormalMatrix * float4(attributes.vertTangentOS, 0)).xyz);
#endif
    vars.UV = attributes.texCoord;

    OUTPUT_CLIP_POS(vars.PositionCS, cameraData._VPMatrix * float4(vars.PositionWS.xyz, 1));
    return vars;
}

half4_type fragmentFunction(Varyings vars, CameraDataStruct cameraData, PerMaterialDataStruct perMaterialData, LightingStruct lightingData,
#ifdef _NORMAL_MAP
                            TEXTURE2D_HALF_PARAMETER(normalMap, normalMapSampler),
#endif
#ifdef _DATA_MAP
                            TEXTURE2D_HALF_PARAMETER(dataMap, dataMapSampler),
#endif
#ifdef _REFLECTION
                            CUBEMAP_HALF_PARAMETER(reflectionCube, reflectionCubeSampler),
#endif
#ifdef _RECEIVE_SHADOWS
                            ShadowsStruct shadowsData,
                            TEXTURE2D_FLOAT_PARAMETER(dirLightShadowMap, dirLightShadowMapSampler),
                            TEXTURE2D_ARRAY_FLOAT_PARAMETER(spotLightShadowMap, spotLightShadowMapSampler),
                            TEXTURE2D_ARRAY_FLOAT_PARAMETER(pointLightShadowMap, pointLightShadowMapSampler),
#endif
                            TEXTURE2D_HALF_PARAMETER(albedoMap, albedoMapSampler))
{
    SETUP_INSTANCE_ID(vars)

#ifdef _NORMAL_MAP
    float3 normalTS = float3(SAMPLE_TEXTURE(normalMap, normalMapSampler, vars.UV * perMaterialData._NormalMap_ST.zw + perMaterialData._NormalMap_ST.xy).rgb);
    float3 normalWS = unpackNormal(normalTS, vars.NormalWS, vars.TangentWS, perMaterialData._NormalIntensity);
#else
    float3 normalWS = normalize(vars.NormalWS);
#endif

#ifdef _DATA_MAP
    half4 data = SAMPLE_TEXTURE(dataMap, dataMapSampler, vars.UV);
    float metallness = data.r;
    float roughness = data.g;
#else
    float metallness = perMaterialData._Metallness;
    float roughness = perMaterialData._Roughness;
#endif

    float4 albedo = float4(SAMPLE_TEXTURE(albedoMap, albedoMapSampler, vars.UV * perMaterialData._Albedo_ST.zw + perMaterialData._Albedo_ST.xy));
#ifdef _PER_INSTANCE_DATA
    albedo *= GET_PER_INSTANCE_VALUE(_Color);
#endif

#ifdef _REFLECTION
    half3 reflection = sampleReflection(PASS_TEXTURE_PARAMETER(reflectionCube, reflectionCubeSampler), perMaterialData._ReflectionCubeLevels, normalWS, vars.PositionWS.xyz, roughness, cameraData._CameraPosWS);
#else
    half3 reflection = half3(0);
#endif

    half3 finalColor = half3(getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, roughness, metallness, reflection, cameraData._CameraPosWS, lightingData
#ifdef _RECEIVE_SHADOWS
                                        ,shadowsData
                                        ,PASS_TEXTURE_PARAMETER(dirLightShadowMap, dirLightShadowMapSampler)
                                        ,PASS_TEXTURE_PARAMETER(spotLightShadowMap, spotLightShadowMapSampler)
                                        ,PASS_TEXTURE_PARAMETER(pointLightShadowMap, pointLightShadowMapSampler)
#endif
                                         ));
    return half4(finalColor.rgb, albedo.a);
}
