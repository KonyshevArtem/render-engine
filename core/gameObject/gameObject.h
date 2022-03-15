#ifndef OPENGL_STUDY_GAMEOBJECT_H
#define OPENGL_STUDY_GAMEOBJECT_H

#include "matrix4x4/matrix4x4.h"
#include "quaternion/quaternion.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <vector>

class Renderer;

class GameObject: public std::enable_shared_from_this<GameObject>
{
public:
    static std::shared_ptr<GameObject> Create(const std::string &_name);
    void                               Destroy();

    std::string                              Name;
    std::vector<std::shared_ptr<GameObject>> Children;
    std::shared_ptr<Renderer>                Renderer;

    std::shared_ptr<GameObject> GetParent() const;
    void                        SetParent(const std::shared_ptr<GameObject> &_gameObject, int _index = -1);

    // global
    void SetPosition(const Vector3 &_position);
    void SetRotation(const Quaternion &_rotation);

    Vector3    GetPosition();
    Quaternion GetRotation();
    Vector3    GetLossyScale();

    // local
    void SetLocalPosition(const Vector3 &_position);
    void SetLocalRotation(const Quaternion &_rotation);
    void SetLocalScale(const Vector3 &_scale);

    const Matrix4x4 &GetLocalToWorldMatrix();
    const Matrix4x4 &GetWorldToLocalMatrix();

    inline const Vector3 &GetLocalPosition() const
    {
        return m_LocalPosition;
    };

    inline const Quaternion &GetLocalRotation() const
    {
        return m_LocalRotation;
    };


    inline const Vector3 &GetLocalScale() const
    {
        return m_LocalScale;
    };

    // helpers
    bool IsParent(const std::shared_ptr<GameObject> &_child) const;

private:
    GameObject(const std::string &_name);
    GameObject(const GameObject &) = delete;
    GameObject(GameObject &&)      = delete;

    GameObject &operator=(const GameObject &) = delete;
    GameObject &operator=(GameObject &&) = delete;

    std::weak_ptr<GameObject> m_Parent;
    Vector3                   m_LocalPosition;
    Quaternion                m_LocalRotation      = Quaternion();
    Quaternion                m_Rotation           = Quaternion();
    Vector3                   m_LocalScale         = Vector3::One();
    Matrix4x4                 m_LocalToWorldMatrix = Matrix4x4::Identity();
    Matrix4x4                 m_WorldToLocalMatrix = Matrix4x4::Identity();
    bool                      m_DirtyTransform     = true;

    void InvalidateTransform();
    void ValidateTransform();
};

#endif //OPENGL_STUDY_GAMEOBJECT_H
