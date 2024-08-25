layout(location = 0) in vec3 vertPositionOS;
layout(location = 2) in vec2 texcoord;

#include "outlineBlit_shared.h"

out Varyings vars;

void main()
{
	Attributes attributes;
	attributes.PositionOS = vertPositionOS;
	attributes.TexCoord = texcoord;

	vars = vertexFunction(attributes);
}