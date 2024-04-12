#include "skybox_shared.h"

Varyings vertex vertexMain( Attributes input [[stage_in]],
                            constant PerDrawDataStruct & PerDrawData [[buffer(1)]],
                            constant CameraDataStruct & CameraData [[buffer(2)]])
{
    return vertexFunction(input, PerDrawData, CameraData);
}

half4 fragment fragmentMain(Varyings input [[stage_in]],
                            texturecube<half> _Skybox [[texture (0)]],
                            sampler _SkyboxSampler [[sampler (0)]])
{
    return fragmentFunction(input, _Skybox, _SkyboxSampler);
}