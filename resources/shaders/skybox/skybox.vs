layout(location = 0) in vec3 vertPositionOS;

#include "skybox_shared.h"

out Varyings vars;

DECLARE_UBO(CameraData);
DECLARE_UBO(PerDrawData);

void main()
{
    Attributes attributes;
    attributes.positionOS = vertPositionOS;

    vars = vertexFunction(attributes, PerDrawDataVar, CameraDataVar);
}