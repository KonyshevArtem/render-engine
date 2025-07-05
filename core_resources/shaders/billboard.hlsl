#include "common/per_draw_data.h"
#include "common/camera_data.h"

struct Attributes
{
    float3 PositionWS : POSITION;
};

struct Varyings
{
    float4 PositionCS     : SV_POSITION;
    float2 Uv             : TEXCOORD;
};

cbuffer PerMaterialData : register(b4)
{
    float2 _Size;
    float2 _Padding0;
};

Texture2D _Texture : register(t4);
SamplerState sampler_Texture : register(s4);

Varyings vertexMain(Attributes input, uint vid : SV_VertexID)
{
    float4 posWS = mul(_ModelMatrix, float4(input.PositionWS, 1));
    float3 cameraFwdProjected = normalize(float3(_CameraFwdWS.x, 0, _CameraFwdWS.z));

    float3 up = float3(0, 1, 0);
    float3 right = cross(cameraFwdProjected, up);

    up *= _Size.y;
    right *= _Size.x;

    Varyings vars;

    if (vid == 0)
    {
        vars.PositionCS = mul(_VPMatrix, posWS + float4(-right + up * 2, 0));
        vars.Uv = float2(1, 0);
    }
    else if (vid == 1)
    {
        vars.PositionCS = mul(_VPMatrix, posWS + float4(-right, 0));
        vars.Uv = float2(1, 1);
    }
    else if (vid == 2)
    {
        vars.PositionCS = mul(_VPMatrix, posWS + float4(right + up * 2, 0));
        vars.Uv = float2(0, 0);
    }
    else
    {
        vars.PositionCS = mul(_VPMatrix, posWS + float4(right, 0));
        vars.Uv = float2(0, 1);
    }

    return vars;
}

half4 fragmentMain(Varyings input) : SV_Target
{
    float4 texColor = _Texture.Sample(sampler_Texture, input.Uv);
    if (texColor.a < 0.1)
        discard;
    return texColor;
}