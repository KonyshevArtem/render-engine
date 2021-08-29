#include "common/attributes.glsl"
#include "common/camera_data.glsl"

#ifdef VERTEX
void main(){
    gl_Position = _ProjMatrix * _ViewMatrix * _ModelMatrix * vertPositionOS;
}
#endif //VERTEX

#ifdef FRAGMENT
out vec4 color;

void main(){
    color = vec4(1, 0, 1, 1);
}
#endif //FRAGMENT