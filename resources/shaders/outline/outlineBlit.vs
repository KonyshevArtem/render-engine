layout(location = 0) in vec3 vertPositionOS;
layout(location = 2) in vec2 texcoord;

#include "outlineBlit_per_material.cg"

out vec2[5] uvs;

void main(){
    gl_Position = vec4(vertPositionOS, 1);
    
    uvs[0] = texcoord + _BlitTexture_TexelSize.zw * vec2(2, 0);
	uvs[1] = texcoord + _BlitTexture_TexelSize.zw * vec2(-2, 0);
	uvs[2] = texcoord + _BlitTexture_TexelSize.zw * vec2(0, 2);
	uvs[3] = texcoord + _BlitTexture_TexelSize.zw * vec2(0, -2);
	uvs[4] = texcoord;
}