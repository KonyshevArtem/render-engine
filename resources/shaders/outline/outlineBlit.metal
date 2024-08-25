#include "outlineBlit_shared.h"

Varyings vertex vertexMain( Attributes input [[stage_in]])
{
    return vertexFunction(input);
}

half4 fragment fragmentMain(Varyings input [[stage_in]],
                            constant PerMaterialDataStruct & PerMaterialData [[buffer (1)]],
                            texture2d<half> _BlitTexture [[texture (0)]],
                            sampler _BlitTextureSampler [[sampler (0)]])
{
    return fragmentFunction(input, PerMaterialData, _BlitTexture, _BlitTextureSampler);
}