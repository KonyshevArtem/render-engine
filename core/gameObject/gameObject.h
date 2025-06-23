#ifndef RENDER_ENGINE_GAMEOBJECT_H
#define RENDER_ENGINE_GAMEOBJECT_H

#include "matrix4x4/matrix4x4.h"
#include "quaternion/quaternion.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <vector>

class Renderer;
class Component;
class Scene;

class GameObject: public std::enable_shared_from_this<GameObject>
{
public:
    static std::shared_ptr<GameObject> Create(const std::string& name, std::shared_ptr<Scene> scene = nullptr);
    void Destroy();

    std::string Name;
    std::vector<std::shared_ptr<GameObject>> Children;

    std::shared_ptr<GameObject> GetParent() const;
    void                        SetParent(const std::shared_ptr<GameObject> &newParent, int _index = -1);

    void AddComponent(std::shared_ptr<Component> component);

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

    inline int GetUniqueID() const
    {
        return m_UniqueId;
    }

    inline std::shared_ptr<Renderer> GetRenderer()
    {
        return m_Renderer.expired() ? nullptr : m_Renderer.lock();
    }

    // helpers
    bool IsParent(const std::shared_ptr<GameObject> &_child) const;

    GameObject(const GameObject &) = delete;
    GameObject(GameObject &&) = delete;

    GameObject &operator=(const GameObject &) = delete;
    GameObject &operator=(GameObject &&) = delete;

private:
    explicit GameObject(std::string name, std::shared_ptr<Scene> scene);

    std::weak_ptr<GameObject> m_Parent;
    std::weak_ptr<Scene> m_Scene;
    std::weak_ptr<Renderer> m_Renderer;
    std::vector<std::shared_ptr<Component>> m_Components;
    Vector3 m_LocalPosition;
    Quaternion m_LocalRotation = Quaternion();
    Quaternion m_Rotation = Quaternion();
    Vector3 m_LocalScale = Vector3::One();
    Matrix4x4 m_LocalToWorldMatrix = Matrix4x4::Identity();
    Matrix4x4 m_WorldToLocalMatrix = Matrix4x4::Identity();
    bool m_DirtyTransform = true;
    int m_UniqueId = -1;

    void InvalidateTransform();
    void ValidateTransform();
};

#endif //RENDER_ENGINE_GAMEOBJECT_H
