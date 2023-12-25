uniform samplerCube _Skybox;

in vec3 CubemapUV;
out vec4 outColor;

void main(){
    outColor = textureLod(_Skybox, CubemapUV, 0);
}