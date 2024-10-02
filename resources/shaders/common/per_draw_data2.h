#ifndef RENDER_ENGINE_PER_DRAW_DATA
#define RENDER_ENGINE_PER_DRAW_DATA

cbuffer PerDrawData
{
    float4x4 _ModelMatrix;
    float4x4 _ModelNormalMatrix;
};

#endif