#include "gameObject.h"
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
    auto parent = GetParent();
    if (!parent && !Scene::Current)
        return;

    auto &collection = parent ? parent->Children : Scene::Current->m_GameObjects;
    RemoveGameObjectFromCollection(this, collection);
}

GameObject::GameObject(const std::string &_name) :
    Name(_name)
{
}

std::shared_ptr<GameObject> GameObject::GetParent() const
{
    return !m_Parent.expired() ? m_Parent.lock() : nullptr;
}

void GameObject::SetParent(const std::shared_ptr<GameObject> &_gameObject, int _index)
{
    if (_gameObject.get() == this || IsParent(_gameObject))
        return;

    auto oldParent = GetParent();
    if (oldParent == _gameObject && _index < 0)
        return;

    auto thisPtr = shared_from_this(); // make pointer copy before removing gameObject from children
    auto pos     = GetPosition();
    auto rot     = GetRotation();

    auto &oldCollection = oldParent ? oldParent->Children : Scene::Current->m_GameObjects;
    RemoveGameObjectFromCollection(this, oldCollection);

    m_Parent = _gameObject;

    auto &newCollection = _gameObject ? _gameObject->Children : Scene::Current->m_GameObjects;
    if (_index < 0 || _index >= newCollection.size())
        newCollection.push_back(thisPtr);
    else
    {
        auto it = newCollection.begin() + _index;
        newCollection.insert(it, thisPtr);
    }

    SetPosition(pos);
    SetRotation(rot);
}

// global
void GameObject::SetPosition(const Vector3 &_position)
{
    auto parent = GetParent();
    if (!parent)
        SetLocalPosition(_position);
    else
        SetLocalPosition(parent->GetWorldToLocalMatrix() * _position.ToVector4(1));
}

void GameObject::SetRotation(const Quaternion &_rotation)
{
    auto parent = GetParent();
    if (!parent)
        SetLocalRotation(_rotation);
    else
        SetLocalRotation(parent->GetRotation().Inverse() * _rotation);
}

Vector3 GameObject::GetPosition()
{
    return GetLocalToWorldMatrix().GetPosition();
}

Quaternion GameObject::GetRotation()
{
    ValidateTransform();
    return m_Rotation;
}

Vector3 GameObject::GetLossyScale()
{
    auto parent = GetParent();
    return parent ? parent->GetLossyScale() * m_LocalScale : m_LocalScale;
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

    auto parent = GetParent();
    if (parent)
        m_LocalToWorldMatrix = parent->GetLocalToWorldMatrix() * m_LocalToWorldMatrix;

    m_WorldToLocalMatrix = m_LocalToWorldMatrix.Invert();

    m_Rotation = parent ? parent->GetRotation() * m_LocalRotation : m_LocalRotation;
}

// helpers
bool GameObject::IsParent(const std::shared_ptr<GameObject> &_child) const
{
    if (!_child)
        return false;

    auto parent = _child->GetParent();
    if (!parent)
        return false;

    if (parent.get() == this)
        return true;

    return IsParent(parent);
}