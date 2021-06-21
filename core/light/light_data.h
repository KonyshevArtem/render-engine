#ifndef OPENGL_STUDY_LIGHT_DATA_H
#define OPENGL_STUDY_LIGHT_DATA_H

#include "../../math/vector3/vector3.h"
#include "../../math/vector4/vector4.h"

struct LightData
{
public:
    Vector3 PosOrDirWS;

private:
    float p1; // padding

public:
    Vector4 Intensity;
    int     IsDirectional;
    float   Attenuation;

private:
    float p3[2]; // padding
};

#endif //OPENGL_STUDY_LIGHT_DATA_H
