layout(location = 0) in vec3 vertPositionOS;

#include "fallback_shared.h"

DECLARE_UBO(PerDrawData);
DECLARE_UBO(CameraData);

void main(){
    Attributes attributes;
    attributes.positionOS = vertPositionOS;

    gl_Position = vertexFunction(attributes, PerDrawDataVar, CameraDataVar);
}