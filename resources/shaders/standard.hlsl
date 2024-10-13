#include "common/camera_data.h"
#include "common/per_draw_data.h"
#include "common/lighting.h"

//#ifdef _PER_INSTANCE_DATA
//    BEGIN_PER_INSTANCE_DATA
//        float4 _Color;
//        float _Size;
//    END_PER_INSTANCE_DATA
//#endif

struct Attributes
{
    float3 vertPositionOS   : POSITION;
    float3 vertNormalOS     : NORMAL;
    float2 texCoord         : TEXCOORD;
    float3 vertTangentOS    : TANGENT;
};

struct Varyings
{
    float4 PositionCS       : SV_POSITION;
    float3 PositionWS       : TEXCOORD0;
    float3 NormalWS         : TEXCOORD1;
    float2 UV               : TEXCOORD2;
#ifdef _NORMAL_MAP
    float3 TangentWS        : TEXCOORD3;
#endif
    //DECLARE_INSTANCE_ID_VARYING
};

cbuffer PerMaterialData
{
    float4 _Albedo_ST;
    float4 _NormalMap_ST;
    float _Roughness;
    float _Metallness;
    float _NormalIntensity;
    float _ReflectionCubeLevels;
};

Texture2D _Albedo;
SamplerState sampler_Albedo;

Texture2D _NormalMap;
SamplerState sampler_NormalMap;

Texture2D _Data;
SamplerState sampler_Data;

Varyings vertexMain(Attributes attributes, uint instanceID : SV_InstanceID)
{
    Varyings vars;

//    TRANSFER_INSTANCE_ID_VARYING(vars)
    SETUP_INSTANCE_ID(instanceID)

    float3 vertPos = attributes.vertPositionOS;
//#ifdef _PER_INSTANCE_DATA
//    vertPos *= GET_PER_INSTANCE_VALUE(_Size);
//#endif

    vars.PositionWS = mul(_ModelMatrix, float4(vertPos, 1)).xyz;
    vars.NormalWS = normalize(mul(_ModelNormalMatrix, float4(attributes.vertNormalOS, 0)).xyz);
#ifdef _NORMAL_MAP
    vars.TangentWS = normalize(mul(_ModelNormalMatrix, float4(attributes.vertTangentOS, 0)).xyz);
#endif
    vars.UV = attributes.texCoord;

    vars.PositionCS = mul(_VPMatrix, float4(vars.PositionWS.xyz, 1));
    return vars;
}

half4 fragmentMain(Varyings vars) : SV_Target
{
    //SETUP_INSTANCE_ID(vars)

#ifdef _NORMAL_MAP
    float3 normalTS = (float3) _NormalMap.Sample(sampler_NormalMap, vars.UV * _NormalMap_ST.zw + _NormalMap_ST.xy).rgb;
    float3 normalWS = unpackNormal(normalTS, vars.NormalWS, vars.TangentWS, _NormalIntensity);
#else
    float3 normalWS = normalize(vars.NormalWS);
#endif

#ifdef _DATA_MAP
    half4 data = _Data.Sample(sampler_Data, vars.UV);
    float metallness = data.r;
    float roughness = data.g;
#else
    float metallness = _Metallness;
    float roughness = _Roughness;
#endif

    float4 albedo = float4(_Albedo.Sample(sampler_Albedo, vars.UV * _Albedo_ST.zw + _Albedo_ST.xy));
//#ifdef _PER_INSTANCE_DATA
//    albedo *= GET_PER_INSTANCE_VALUE(_Color);
//#endif

#ifdef _REFLECTION
    half3 reflection = sampleReflection(_ReflectionCubeLevels, normalWS, vars.PositionWS.xyz, roughness, _CameraPosWS);
#else
    half3 reflection = (half3) 0.0h;
#endif

    half3 finalColor = (half3) getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, roughness, metallness, reflection, _CameraPosWS);
    return half4(finalColor.rgb, albedo.a);
}
