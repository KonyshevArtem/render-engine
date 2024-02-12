layout(location = 0) in vec3 vertPositionOS;

out vec3 CubemapUV;

#include "../common/per_instance_data.cg"
#include "../common/camera_data.cg"

void main(){
    gl_Position = _VPMatrix * _ModelMatrix * vec4(vertPositionOS, 1);
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    gl_Position = gl_Position.xyww;

    CubemapUV = vertPositionOS;
}