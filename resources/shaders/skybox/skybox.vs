layout(location = 0) in vec3 vertPositionOS;

out vec3 CubemapUV;

uniform mat4 _ModelMatrix;

#include "../common/camera_data.cg"

void main(){
    gl_Position = _ProjMatrix * _ViewMatrix * _ModelMatrix * vec4(vertPositionOS, 1);
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    gl_Position = gl_Position.xyww;

    CubemapUV = vertPositionOS;
}