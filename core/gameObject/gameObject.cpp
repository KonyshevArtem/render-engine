#include "gameObject.h"
#include "core_debug/debug.h"
#include "vector4/vector4.h"

std::shared_ptr<GameObject> GameObject::Create(const std::string &_name)
{
    return std::shared_ptr<GameObject>(new GameObject(_name));
}

GameObject::GameObject(const std::string &_name) :
    Name(_name)
{
}

GameObject::~GameObject()
{
    Debug::LogError(Name);
    if (Parent.expired())
        return;

    // TODO
}

void GameObject::SetParent(const std::shared_ptr<GameObject> &_gameObject)
{
    if (Parent.expired())
    {
        Parent = _gameObject;
        if (_gameObject)
            _gameObject->Children.push_back(shared_from_this());
    }

    Parent = _gameObject;

    // TODO
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