#include "common/attributes.glsl"
#include "common/camera_data.glsl"

uniform samplerCube _Skybox;

#pragma vertex vert
#pragma fragment frag

Varyings vert(){
    Varyings vars;
    vars.PositionCS = _ProjMatrix * _ViewMatrix * _ModelMatrix * vec4(vertPositionOS, 1);
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    vars.PositionCS = vars.PositionCS.xyww;
    vars.CubemapUV = vertPositionOS;
    return vars;
}

vec4 frag(Varyings vars){
    return texture(_Skybox, vars.CubemapUV);
}