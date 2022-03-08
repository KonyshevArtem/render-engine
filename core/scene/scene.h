#ifndef OPENGL_STUDY_SCENE_H
#define OPENGL_STUDY_SCENE_H

#include "gameObject/gameObject.h"
#include "vector3/vector3.h"
#include <memory>
#include <string>
#include <vector>

class Light;
class Cubemap;

class Scene
{
public:
    inline static std::shared_ptr<Scene> Current = nullptr;
    std::vector<std::shared_ptr<Light>>  Lights;
    Vector3                              AmbientLight;
    std::shared_ptr<Cubemap>             Skybox;

    static void Update();

    std::vector<std::shared_ptr<GameObject>>::const_iterator cbegin();
    std::vector<std::shared_ptr<GameObject>>::const_iterator cend();

private:
    std::vector<std::shared_ptr<GameObject>> m_GameObjects;

    virtual void UpdateInternal() = 0;

    friend void                        GameObject::SetParent(const std::shared_ptr<GameObject> &, int);
    friend std::shared_ptr<GameObject> GameObject::Create(const std::string &);
    friend void                        GameObject::Destroy();
};

#endif //OPENGL_STUDY_SCENE_H
