#ifndef RENDER_ENGINE_LIGHT_H
#define RENDER_ENGINE_LIGHT_H

#include "quaternion/quaternion.h"
#include "vector3/vector3.h"

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

class Light
{
public:
    Vector3    Position;
    Quaternion Rotation;
    LightType  Type;
    Vector3    Intensity;
    float      Attenuation;
    float      CutOffAngle;
};

#endif //RENDER_ENGINE_LIGHT_H
