layout(location = 0) in vec3 vertPositionOS;

#pragma vertex vert
#pragma geometry geometry/quad.glsl
#pragma fragment frag

struct Varyings{
    vec4 PositionCS;
};

Varyings vert(){
    Varyings vars;
    vars.PositionCS = vec4(vertPositionOS, 1);
    return vars;
}

vec4 frag(Varyings vars){
    return vec4(1, 1, 1, 1);
}