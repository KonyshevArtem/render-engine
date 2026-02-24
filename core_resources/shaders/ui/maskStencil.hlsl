cbuffer UIData : register(b0)
{
    float4 _OffsetScale;
}

struct Attributes
{
    float3 PositionOS : POSITION;
};

struct Varyings
{
    float4 PositionCS : SV_Position;
};

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;
    vars.PositionCS = float4((attributes.PositionOS.xy * _OffsetScale.zw + _OffsetScale.xy) * 2 - 1, 0.5, 1);
    return vars;
}

float4 fragmentMain(Varyings vars) : SV_Target
{
    return float4(0, 0, 0, 0);
}