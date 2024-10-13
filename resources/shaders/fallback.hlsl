#include "common/camera_data.h"
#include "common/per_draw_data.h"

struct Attributes
{
  float3 positionOS : POSITION;
};

struct Varyings
{
  float4 positionCS : SV_POSITION;
};

Varyings vertexMain(Attributes attributes)
{
  Varyings result;
  result.positionCS = mul(_VPMatrix, mul(_ModelMatrix, float4(attributes.positionOS, 1.0)));
  return result;
}

half4 fragmentMain() : SV_Target
{
   return half4(1, 0, 1, 1);
}