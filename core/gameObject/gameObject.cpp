#include "gameObject.h"
#include "core_debug/debug.h"
#include "scene/scene.h"
#include "vector4/vector4.h"

std::shared_ptr<GameObject> GameObject::Create(const std::string &_name)
{
    if (!Scene::Current)
        return nullptr;

    auto ptr = std::shared_ptr<GameObject>(new GameObject(_name));
    Scene::Current->m_GameObjects.push_back(ptr);
    return ptr;
}

void RemoveGameObjectFromCollection(GameObject *_go, std::vector<std::shared_ptr<GameObject>> &_collection)
{
    for (auto it = _collection.cbegin(); it != _collection.cend(); it++)
    {
        if (it->get() == _go)
        {
            _collection.erase(it);
            return;
        }
    }
}

void GameObject::Destroy()
{
    bool hasParent = !Parent.expired();
    if (!hasParent && !Scene::Current)
        return;

    auto  parent     = hasParent ? Parent.lock() : nullptr;
    auto &collection = hasParent ? parent->Children : Scene::Current->m_GameObjects;
    RemoveGameObjectFromCollection(this, collection);
}

GameObject::GameObject(const std::string &_name) :
    Name(_name)
{
}

void GameObject::SetParent(const std::shared_ptr<GameObject> &_gameObject)
{
    if (_gameObject.get() == this)
        return;

    bool hadParent = !Parent.expired();
    auto oldParent = hadParent ? Parent.lock() : nullptr;
    if (oldParent == _gameObject)
        return;

    Parent = _gameObject;

    auto &newCollection = _gameObject ? _gameObject->Children : Scene::Current->m_GameObjects;
    newCollection.push_back(shared_from_this());

    auto &oldCollection = hadParent ? oldParent->Children : Scene::Current->m_GameObjects;
    RemoveGameObjectFromCollection(this, oldCollection);

    InvalidateTransform();
}

// global
void GameObject::SetPosition(const Vector3 &_position)
{
    if (Parent.expired())
        SetLocalPosition(_position);
    else
        SetLocalPosition(Parent.lock()->GetWorldToLocalMatrix() * _position.ToVector4(1));
}

void GameObject::SetRotation(const Quaternion &_rotation)
{
    if (Parent.expired())
        SetLocalRotation(_rotation);
    else
        SetLocalRotation(Parent.lock()->GetWorldToLocalMatrix().GetRotation() * _rotation);
}

Vector3 GameObject::GetPosition()
{
    return GetLocalToWorldMatrix().GetPosition();
}


Quaternion GameObject::GetRotation()
{
    return GetLocalToWorldMatrix().GetRotation();
}


Vector3 GameObject::GetLossyScale()
{
    return GetLocalToWorldMatrix().GetScale();
}

// local

const Matrix4x4 &GameObject::GetLocalToWorldMatrix()
{
    ValidateTransform();
    return m_LocalToWorldMatrix;
}

const Matrix4x4 &GameObject::GetWorldToLocalMatrix()
{
    ValidateTransform();
    return m_WorldToLocalMatrix;
}

void GameObject::SetLocalPosition(const Vector3 &_position)
{
    m_LocalPosition = _position;
    InvalidateTransform();
}

void GameObject::SetLocalRotation(const Quaternion &_rotation)
{
    m_LocalRotation = _rotation;
    InvalidateTransform();
}

void GameObject::SetLocalScale(const Vector3 &_scale)
{
    m_LocalScale = _scale;
    InvalidateTransform();
}

void GameObject::InvalidateTransform()
{
    m_DirtyTransform = true;

    for (const auto &child: Children)
    {
        if (child)
            child->InvalidateTransform();
    }
}

void GameObject::ValidateTransform()
{
    if (!m_DirtyTransform)
        return;

    m_DirtyTransform     = false;
    m_LocalToWorldMatrix = Matrix4x4::TRS(m_LocalPosition, m_LocalRotation, m_LocalScale);
    if (!Parent.expired())
        m_LocalToWorldMatrix = Parent.lock()->GetLocalToWorldMatrix() * m_LocalToWorldMatrix;

    m_WorldToLocalMatrix = m_LocalToWorldMatrix.Invert();
}