layout(location = 0) in vec3 vertPositionWS;

#include "../common/per_draw_data.cg"

void main(){
    gl_Position = _ModelMatrix * vec4(vertPositionWS, 1);
}