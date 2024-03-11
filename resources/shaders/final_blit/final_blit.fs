in vec2 uv;

uniform sampler2D _BlitTexture;

out vec4 outColor;

void main()
{
    outColor = texture(_BlitTexture, uv);
}