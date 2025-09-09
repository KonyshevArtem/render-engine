#include "common/camera_data.h"
#include "common/per_draw_data.h"
#include "common/lighting.h"

struct Attributes
{
    float3 vertPositionOS   : POSITION;
    float3 vertNormalOS     : NORMAL;
    float2 texCoord         : TEXCOORD;
    float3 vertTangentOS    : TANGENT;
    DECLARE_INSTANCE_ID_ATTRIBUTE()
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
    DECLARE_INSTANCE_ID_VARYING(4)
};

cbuffer PerMaterialData : register(b4)
{
    float4 _Albedo_ST;
    float4 _NormalMap_ST;
    float _Roughness;
    float _Metallness;
#ifdef _ALPHA_CLIP
    float _AlphaClip;
    float _Unused0;
#else
    float2 _Unused0;
#endif
};

Texture2D _Albedo : register(t4);
SamplerState sampler_Albedo : register(s4);

Texture2D _NormalMap : register(t5);
SamplerState sampler_NormalMap : register(s5);

Texture2D _Data : register(t6);
SamplerState sampler_Data : register(s6);

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;

    SETUP_INSTANCE_ID(attributes)
    TRANSFER_INSTANCE_ID_VARYING(vars)

    vars.PositionWS = mul(_ModelMatrix, float4(attributes.vertPositionOS, 1)).xyz;
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
    SETUP_INSTANCE_ID(vars)

#ifdef _NORMAL_MAP
    float2 normalTS = (float2) _NormalMap.Sample(sampler_NormalMap, vars.UV * _NormalMap_ST.zw + _NormalMap_ST.xy).rg;
    float3 normalWS = unpackNormal(normalTS, vars.NormalWS, vars.TangentWS);
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
#ifdef _ALPHA_CLIP
    if (albedo.a < _AlphaClip)
        discard;
#endif

#ifdef _REFLECTION
    half3 reflection = sampleReflection(normalWS, vars.PositionWS.xyz, roughness, _CameraPosWS);
#else
    half3 reflection = (half3) 0.0h;
#endif

    half3 finalColor = (half3) getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, roughness, metallness, reflection, _CameraPosWS);
    return half4(finalColor.rgb, albedo.a);
}
