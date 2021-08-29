#include "../attributes.glsl"
#include "../camera_data.glsl"

void main(){
    gl_Position = _ProjMatrix * _ViewMatrix * _ModelMatrix * vertPositionOS;
}