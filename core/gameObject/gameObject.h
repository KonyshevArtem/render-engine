#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "../../math/quaternion/quaternion.h"
#include "../../math/vector3/vector3.h"
#include <vector>

class Renderer;

using namespace std;

class GameObject
{
public:
    weak_ptr<GameObject>           Parent;
    vector<shared_ptr<GameObject>> Children;
    Vector3                        LocalPosition;
    Quaternion                     LocalRotation;
    Vector3                        LocalScale;
    shared_ptr<Renderer>           Renderer;

    GameObject();
};

#endif //OPENGL_STUDY_GAMEOBJECT_H
