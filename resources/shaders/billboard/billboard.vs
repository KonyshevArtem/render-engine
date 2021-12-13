layout(location = 0) in vec3 vertPositionWS;

void main(){
    gl_Position = vec4(vertPositionWS, 1);
}