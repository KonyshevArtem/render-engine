#include "final_blit_shared.h"

in Varyings vars;

uniform sampler2D _BlitTexture;

out vec4 outColor;

DECLARE_UBO(PerMaterialData);

void main()
{
    outColor = fragmentFunction(vars, PerMaterialDataVar, _BlitTexture);
}