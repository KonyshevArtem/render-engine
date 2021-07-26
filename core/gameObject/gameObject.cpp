#include "gameObject.h"

GameObject::GameObject()
{
    LocalScale = Vector3::One();
    Parent     = nullptr;
    Mesh       = nullptr;
    Material   = nullptr;
}
