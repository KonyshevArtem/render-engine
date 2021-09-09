#include "common/attributes.glsl"
#include "common/camera_data.glsl"

#define VERTEX vert
#define FRAGMENT frag

VARYINGS vert(){
    VARYINGS vars;
    vars.PositionCS = _ProjMatrix * _ViewMatrix * _ModelMatrix * vertPositionOS;
    return vars;
}

vec4 frag(VARYINGS vars){
    return vec4(1);
}