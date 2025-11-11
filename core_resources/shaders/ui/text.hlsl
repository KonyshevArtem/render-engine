cbuffer UIData : register(b0)
{
    float4 _OffsetScale;
    float4 _Color;
}

Texture2D _FontAtlas : register(t0);
SamplerState sampler_Image : register(s0);

struct Attributes
{
    float3 PositionOS : POSITION;
    float2 UV : TEXCOORD;
};

struct Varyings
{
    float4 PositionCS : SV_Position;
    float2 UV : TEXCOORD;
};

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;
    vars.PositionCS = float4((attributes.PositionOS.xy * _OffsetScale.zw + _OffsetScale.xy) * 2 - 1, 0.5, 1);
    vars.UV = attributes.UV;
    return vars;
}

float4 fragmentMain(Varyings vars) : SV_Target
{
    float4 color = _Color;
    color.a *= 1 - _FontAtlas.Sample(sampler_Image, vars.UV).r;
    return color;
}