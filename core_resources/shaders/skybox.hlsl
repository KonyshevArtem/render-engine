struct Attributes
{
    float3 positionOS : POSITION;
};

struct Varyings
{
    float4 positionCS : SV_POSITION;
    float3 CubemapUV : TEXCOORD0;
};

cbuffer SkyboxData : register(b0)
{
    float4x4 _MVPMatrix;
}

TextureCube _Skybox : register(t0);
SamplerState sampler_Skybox : register(s0);

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;
    vars.positionCS = mul(_MVPMatrix, float4(attributes.positionOS, 1.0));
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    vars.positionCS = vars.positionCS.xyww;

    vars.CubemapUV = attributes.positionOS.xyz;
    return vars;
}

half4 fragmentMain(Varyings vars) : SV_Target
{
    return _Skybox.SampleLevel(sampler_Skybox, vars.CubemapUV, 0);
}
