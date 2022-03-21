layout(location = 0) in vec3 vertPositionOS;
layout(location = 2) in vec2 texcoord;

out vec2 uv;

void main(){
    gl_Position = vec4(vertPositionOS, 1);
    uv = texcoord;
}