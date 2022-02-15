layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform vec2 _Size;

#include "../common/camera_data.cg"

out vec2 uv;

void main(){
    mat4 mvp = _ProjMatrix * _ViewMatrix;

    vec3 camForward = (inverse(_ViewMatrix) * vec4(0, 0, 1, 0)).xyz;
    camForward.y = 0;
    camForward = normalize(camForward);

    vec3 up = vec3(0, 1, 0);
    vec3 right = cross(camForward, up);

    up *= _Size.y;
    right *= _Size.x;

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(-right + up * 2, 0));
    uv = vec2(1, 0);
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(-right, 0));
    uv = vec2(1, 1);
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(right + up * 2, 0));
    uv = vec2(0, 0);
    EmitVertex();

    gl_Position = mvp * (gl_in[0].gl_Position + vec4(right, 0));
    uv = vec2(0, 1);
    EmitVertex();
}