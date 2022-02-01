#include "gameObject.h"

GameObject::GameObject() :
    LocalScale(Vector3::One()), Parent(std::shared_ptr<GameObject>(nullptr)), Renderer(nullptr)
{
}
