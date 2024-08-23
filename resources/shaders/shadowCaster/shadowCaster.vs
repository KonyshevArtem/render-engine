layout(location = 0) in vec3 vertPositionOS;

#include "shadowCaster_shared.h"

DECLARE_UBO(CameraData);
DECLARE_UBO(PerDrawData);

void main()
{
    Attributes attributes;
    attributes.PositionOS = vertPositionOS;

    gl_Position = vertexFunction(attributes, CameraDataVar, PerDrawDataVar);
}