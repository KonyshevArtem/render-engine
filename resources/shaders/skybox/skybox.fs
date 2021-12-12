uniform samplerCube _Skybox;

in vec3 CubemapUV;
out vec4 outColor;

void main(){
    outColor = texture(_Skybox, CubemapUV);
}