layout(location = 0) in vec3 vertPositionOS;
layout(location = 1) in vec3 vertNormalOS;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertTangentOS;

#include "standard_shared.h"

out Varyings vars;

DECLARE_UBO(PerDrawData);
DECLARE_UBO(CameraData);

void main(){
    Attributes attributes;
    attributes.vertPositionOS = vertPositionOS;
    attributes.vertNormalOS = vertNormalOS;
    attributes.texCoord = texCoord;
    attributes.vertTangentOS = vertTangentOS;

    vars = vertexFunction(attributes, PerDrawDataVar, CameraDataVar);
}