#include "common/attributes.glsl"
#include "common/camera_data.glsl"

#pragma vertex vert
#pragma fragment frag

Varyings vert(){
    Varyings vars;
    vars.PositionCS = _ProjMatrix * _ViewMatrix * _ModelMatrix * vec4(vertPositionOS, 1);
    return vars;
}

vec4 frag(Varyings vars){
    return vec4(1, 0, 1, 1);
}