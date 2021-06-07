#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "../../math/quaternion/quaternion.h"
#include "../../math/vector4/vector4.h"

typedef struct gameObject
{
    struct gameObject *parent;
    vector4            localPosition;
    quaternion         localRotation;
    vector4            localScale;
} gameObject;

#endif//OPENGL_STUDY_GAMEOBJECT_H
