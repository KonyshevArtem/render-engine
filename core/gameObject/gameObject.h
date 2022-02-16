#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "quaternion/quaternion.h"
#include "vector3/vector3.h"
#include <vector>
#include <memory>

class Renderer;

class GameObject
{
public:
    std::weak_ptr<GameObject>                Parent;
    std::vector<std::shared_ptr<GameObject>> Children;
    Vector3                                  LocalPosition;
    Quaternion                               LocalRotation;
    Vector3                                  LocalScale;
    std::shared_ptr<Renderer>                Renderer;

    GameObject();
};

#endif //OPENGL_STUDY_GAMEOBJECT_H
