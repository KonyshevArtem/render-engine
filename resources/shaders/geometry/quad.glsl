layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

struct Varyings{
    vec4 PositionCS;
};

in Varyings vert_vars[];
out Varyings frag_vars;

void main(){
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
}