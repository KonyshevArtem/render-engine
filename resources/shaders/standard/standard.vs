layout(location = 0) in vec3 vertPositionOS;
layout(location = 1) in vec3 vertNormalOS;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertTangentOS;

#include "../common/per_instance_data.cg"

out Varyings
{
    vec3 PositionWS;
    vec3 NormalWS;
#ifdef _NORMAL_MAP
    vec3 TangentWS;
#endif
    vec2 UV;
} vars;

#include "../common/camera_data.cg"

void main(){
    vars.PositionWS = (_ModelMatrix * vec4(vertPositionOS, 1)).xyz;
    vars.NormalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
#ifdef _NORMAL_MAP
    vars.TangentWS = normalize((_ModelNormalMatrix * vec4(vertTangentOS, 0)).xyz);
#endif
    vars.UV = texCoord;

    gl_Position = _VPMatrix * vec4(vars.PositionWS.xyz, 1);
}