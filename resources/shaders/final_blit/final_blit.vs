layout(location = 0) in vec3 position;

#include "final_blit_shared.h"

out Varyings vars;

void main()
{
    Attributes attributes;
    attributes.positionOS = position;

    vars = vertexFunction(attributes, gl_VertexID);
}