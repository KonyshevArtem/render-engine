#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "../../math/quaternion/quaternion.h"
#include "../../math/vector3/vector3.h"
#include "../mesh/mesh.h"
#include "../shader/shader.h"
#include "vector"

class GameObject
{
public:
    GameObject *              Parent;
    std::vector<GameObject *> Children;
    Vector3                   LocalPosition;
    Quaternion                LocalRotation;
    Vector3                   LocalScale;
    Mesh *                    Mesh;
    Shader *                  Shader;
    float                     Smoothness;

    GameObject();
};

#endif //OPENGL_STUDY_GAMEOBJECT_H
