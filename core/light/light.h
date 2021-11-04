#ifndef OPENGL_STUDY_LIGHT_H
#define OPENGL_STUDY_LIGHT_H

#include "../../math/vector3/vector3.h"
#include "../../math/vector4/vector4.h"

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
    Vector4    Intensity;
    float      Attenuation;
    float      CutOffAngle;
};

#endif //OPENGL_STUDY_LIGHT_H
