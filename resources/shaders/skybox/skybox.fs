uniform samplerCube _Skybox;

#include "skybox_shared.h"

in Varyings vars;
out vec4 outColor;

void main(){
    outColor = fragmentFunction(vars, _Skybox);
}