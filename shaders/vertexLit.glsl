#include "common/attributes.glsl"
#include "common/camera_data.glsl"
#include "common/lighting.glsl"

uniform sampler2D _Albedo;
uniform vec4 _AlbedoST;

smooth VAR vec4 color;
smooth VAR vec2 uv;

#ifdef VERTEX
void main(){
    vec4 vertPositionWS = _ModelMatrix * vertPositionOS;
    gl_Position = _ProjMatrix * _ViewMatrix * vertPositionWS;

    vec3 normalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor * _AmbientLightColor;
    color += vertColor * getLight(vertPositionWS.xyz, normalWS, false, 0, vec3(0, 0, 0));

    uv = texCoord * _AlbedoST.zw + _AlbedoST.xy;
}
#endif //VERTEX

#ifdef FRAGMENT
out vec4 outColor;

void main(){
    outColor = texture(_Albedo, uv) * color;
}
#endif //FRAGMENT