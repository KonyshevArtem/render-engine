#ifndef GBUFFER_COMMON_H
#define GBUFFER_COMMON_H

struct GBufferOutput
{
    float4 AlbedoRoughness : SV_Target0;
    float4 NormalMetallness : SV_Target1;
};

#endif