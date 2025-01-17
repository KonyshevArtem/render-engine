#include "../common/shadows.h"

struct Attributes
{
    float3 vertPositionOS   : POSITION;
    float3 vertNormalOS     : NORMAL;
    float2 texCoord         : TEXCOORD;
};

struct Varyings
{
    float4 PositionCS       : SV_POSITION;
    float2 UV               : TEXCOORD2;
};

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;

    vars.PositionCS = float4((attributes.vertPositionOS.xy + 1.0) * 0.3 - 1.0, 1, 1);
    vars.UV = attributes.texCoord;

    return vars;
}

half4 fragmentMain(Varyings vars) : SV_Target
{
    half4 depth = _DirLightShadowMap.Sample(sampler_DirLightShadowMap, vars.UV);
    return half4(depth.x, 0, 0, 1.0);
}