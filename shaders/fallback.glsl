#include "common/attributes.glsl"
#include "common/camera_data.glsl"

#define VERTEX vert
#define FRAGMENT frag

Varyings vert(){
    Varyings vars;
    vars.PositionCS = _ProjMatrix * _ViewMatrix * _ModelMatrix * vertPositionOS;
    return vars;
}

vec4 frag(Varyings vars){
    return vec4(1, 0, 1, 1);
}