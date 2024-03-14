layout(location = 0) in vec3 vertPositionOS;
layout(location = 1) in vec3 vertNormalOS;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertTangentOS;

#include "../common/per_draw_data.cg"
#include "../common/camera_data.cg"
#include "standard_shared.cg"

void main(){
    TRANSFER_INSTANCE_ID_VARYING(vars)
    SETUP_INSTANCE_ID

    vec3 vertPos = vertPositionOS;
#ifdef _PER_INSTANCE_DATA
    vertPos *= GET_PER_INSTANCE_VALUE(_Size);
#endif

    vars.PositionWS = (_ModelMatrix * vec4(vertPos, 1)).xyz;
    vars.NormalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
#ifdef _NORMAL_MAP
    vars.TangentWS = normalize((_ModelNormalMatrix * vec4(vertTangentOS, 0)).xyz);
#endif
    vars.UV = texCoord;

    gl_Position = _VPMatrix * vec4(vars.PositionWS.xyz, 1);
}