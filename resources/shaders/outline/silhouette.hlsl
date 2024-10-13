#include "../common/per_draw_data2.h"
#include "../common/camera_data2.h"

struct Attributes
{
    float3 PositionOS : POSITION;
};

float4 vertexMain(Attributes attributes) : SV_Position
{
    return mul(_VPMatrix, mul(_ModelMatrix, float4(attributes.PositionOS, 1)));
}

half4 fragmentMain() : SV_Target
{
    return (half4) 1.0h;
}