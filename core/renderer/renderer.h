#ifndef OPENGL_STUDY_RENDERER_H
#define OPENGL_STUDY_RENDERER_H

#include "memory"

class GameObject;
class Shader;

using namespace std;

class Renderer
{
public:
    virtual void Render()                                  = 0;
    virtual void Render(const shared_ptr<Shader> &_shader) = 0;

protected:
    explicit Renderer(shared_ptr<GameObject> _gameObject);

    shared_ptr<GameObject> m_GameObject;
};

#endif //OPENGL_STUDY_RENDERER_H
