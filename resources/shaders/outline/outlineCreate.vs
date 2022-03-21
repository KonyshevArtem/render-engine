layout(location = 0) in vec3 positionOS;
layout(location = 2) in vec2 texcoord;

uniform vec4 _Tex_TexelSize;

out vec2[4] uvs;

void main(){
	gl_Position = vec4(positionOS, 1);

	uvs[0] = texcoord + _Tex_TexelSize.zw * vec2(2, 0);
	uvs[1] = texcoord + _Tex_TexelSize.zw * vec2(-2, 0);
	uvs[2] = texcoord + _Tex_TexelSize.zw * vec2(0, 2);
	uvs[3] = texcoord + _Tex_TexelSize.zw * vec2(0, -2);
}