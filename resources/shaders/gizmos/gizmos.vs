layout(location = 0) in vec3 positionOS;

#include "../common/camera_data.cg"

uniform mat4 _ModelMatrix;

void main(){
	gl_Position = _ProjMatrix * _ViewMatrix * _ModelMatrix * vec4(positionOS, 1);
}