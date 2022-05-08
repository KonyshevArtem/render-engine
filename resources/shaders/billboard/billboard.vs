layout(location = 0) in vec3 vertPositionWS;

uniform mat4 _ModelMatrix;

void main(){
    gl_Position = _ModelMatrix * vec4(vertPositionWS, 1);
}