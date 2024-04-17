#include "final_blit_shared.h"

Varyings vertex vertexMain(Attributes input [[stage_in]], uint vid [[vertex_id]])
{
    return vertexFunction(input, vid);
}

half4 fragment fragmentMain(Varyings input [[stage_in]],
                            texture2d<half> _BlitTexture [[texture (0)]],
                            sampler _BlitSampler [[sampler (0)]],
                            constant PerMaterialDataStruct & PerMaterialData [[buffer(0)]])
{
    return fragmentFunction(input, PerMaterialData, _BlitTexture, _BlitSampler);
}