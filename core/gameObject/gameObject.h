#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "../../math/quaternion/quaternion.h"
#include "../../math/vector4/vector4.h"
#include "vector"

class GameObject
{
    GameObject *              parent;
    std::vector<GameObject *> children;
    Vector4                   localPosition;
    Quaternion                localRotation;
    Vector4                   localScale;
};

#endif//OPENGL_STUDY_GAMEOBJECT_H
