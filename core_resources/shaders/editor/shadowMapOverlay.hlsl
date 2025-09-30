#include "../common/global_defines.h"
#include "../common/shadows.h"

struct Attributes
{
    float3 vertPositionOS   : POSITION;
    float2 texCoord         : TEXCOORD;
};

struct Varyings
{
    float4 PositionCS       : SV_POSITION;
    float2 UV               : TEXCOORD2;
};

cbuffer DebugData : register(b2)
{
    uint _LightType;
    uint _LightIndex;
    uint _PointLightSide;
    float _ScreenAspect;

    float _Scale;
    float _MinDepth;
    float _MaxDepth;
    float Padding0;
}

SamplerState _Sampler : register(s3);

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;

    float2 xy = (attributes.vertPositionOS.xy + 1.0) * _Scale;
    xy.x /= _ScreenAspect;
    xy -= 1.0;

    vars.PositionCS = float4(xy, 1, 1);
    vars.UV = attributes.texCoord;

    return vars;
}


float4 fragmentMain(Varyings vars) : SV_Target
{
    const uint DIRECTIONAL = 0;
    const uint POINT = 1;
    const uint SPOT = 2;

#if SCREEN_UV_UPSIDE_DOWN
    vars.UV.y = 1 - vars.UV.y;
#endif

    float depth = 0;
    if (_LightType == DIRECTIONAL)
        depth = _DirLightShadowMap.Sample(_Sampler, float3(vars.UV, _LightIndex)).x;
    else if (_LightType == POINT)
        depth = _PointLightShadowMapArray.Sample(_Sampler, float3(vars.UV, _LightIndex * 6 + _PointLightSide)).x;
    else if (_LightType == SPOT)
        depth = _SpotLightShadowMapArray.Sample(_Sampler, float3(vars.UV, _LightIndex)).x;

    depth = (depth - _MinDepth) / (_MaxDepth - _MinDepth);

    return float4(depth, 0, 0, 1.0);
}