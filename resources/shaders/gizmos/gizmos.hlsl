#include "../common/per_draw_data.h"
#include "../common/camera_data.h"

struct Attributes
{
    float3 PositionOS : POSITION;
};

float4 vertexMain(Attributes input, uint instanceID : SV_InstanceID) : SV_Position
{
    SETUP_INSTANCE_ID(instanceID)

    return mul(_VPMatrix, mul(_ModelMatrix, float4(input.PositionOS, 1)));
}

half4 fragmentMain() : SV_Target
{
    return (half4) 1.0h;
}