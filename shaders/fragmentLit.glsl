#include "common/attributes.glsl"
#include "common/camera_data.glsl"
#include "common/lighting.glsl"

uniform sampler2D _Albedo;
uniform vec4 _AlbedoST;
uniform float _Smoothness;

smooth VAR vec4 positionWS;
smooth VAR vec3 normalWS;
smooth VAR vec4 color;
smooth VAR vec2 uv;

#ifdef VERTEX
void main(){
    positionWS = _ModelMatrix * vertPositionOS;
    gl_Position = _ProjMatrix * _ViewMatrix * positionWS;
    normalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor;
    uv = texCoord * _AlbedoST.zw + _AlbedoST.xy;
}
#endif //VERTEX

#ifdef FRAGMENT
out vec4 outColor;

void main(){
    vec4 albedoColor = texture(_Albedo, uv) * color;
    outColor = albedoColor * _AmbientLightColor;
    outColor += albedoColor * getLight(positionWS.xyz, normalize(normalWS), true, _Smoothness, _CameraPosWS);
}
#endif //FRAGMENT