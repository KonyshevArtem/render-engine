#ifndef OPENGL_STUDY_SCENE_H
#define OPENGL_STUDY_SCENE_H

#include "../../math/vector4/vector4.h"
#include "memory"
#include "vector"

class Light;
class GameObject;
class Cubemap;

using namespace std;

class Scene
{
public:
    inline static shared_ptr<Scene> Current = nullptr;
    vector<shared_ptr<GameObject>>  GameObjects;
    vector<shared_ptr<Light>>       Lights;
    Vector4                         AmbientLight;
    shared_ptr<Cubemap>             Skybox;

    static void Update();

private:
    virtual void UpdateInternal() = 0;
};

#endif //OPENGL_STUDY_SCENE_H
