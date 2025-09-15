#include "common/per_draw_data.h"
#include "common/camera_data.h"

struct Attributes
{
    float3 PositionOS : POSITION;
    DECLARE_INSTANCE_ID_ATTRIBUTE()
};

float4 vertexMain(Attributes input) : SV_Position
{
    SETUP_INSTANCE_ID(input)

    float4 worldPos = mul(_ModelMatrix, float4(input.PositionOS, 1));
#ifdef _FRUSTUM_GIZMO
    worldPos /= worldPos.w;
#endif

    return mul(_VPMatrix, worldPos);
}

half4 fragmentMain() : SV_Target
{
    return (half4) 1.0h;
}