#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "../../math/quaternion/quaternion.h"
#include "../../math/vector3/vector3.h"
#include "vector"

class GameObject
{
    GameObject *              parent;
    std::vector<GameObject *> children;
    Vector3                   localPosition;
    Quaternion                localRotation;
    Vector3                   localScale;
};

#endif//OPENGL_STUDY_GAMEOBJECT_H
