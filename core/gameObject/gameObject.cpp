#include "gameObject.h"

GameObject::GameObject() :
    LocalScale(Vector3::One()), Parent(shared_ptr<GameObject>(nullptr)), Renderer(nullptr)
{
}
