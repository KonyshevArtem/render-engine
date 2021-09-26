#include "common/attributes.glsl"
#include "common/camera_data.glsl"

uniform samplerCube _Skybox;

#define VERTEX vert
#define FRAGMENT frag

Varyings vert(){
    Varyings vars;
    vars.PositionWS = _ModelMatrix * vec4(vertPositionOS, 1);
    vars.PositionCS = _ProjMatrix * _ViewMatrix * vars.PositionWS;
    vars.PositionCS = vars.PositionCS.xyww;
    vars.CubemapUV = vertPositionOS;
    return vars;
}

vec4 frag(Varyings vars){
    return texture(_Skybox, vars.CubemapUV);
}