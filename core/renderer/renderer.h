#ifndef OPENGL_STUDY_RENDERER_H
#define OPENGL_STUDY_RENDERER_H

#include <memory>

class GameObject;
class Shader;

using namespace std;

class Renderer
{
public:
    virtual void Render() const                      = 0;
    virtual void Render(const Shader &_shader) const = 0;

protected:
    explicit Renderer(const shared_ptr<GameObject> &_gameObject);

    weak_ptr<GameObject> m_GameObject;
};

#endif //OPENGL_STUDY_RENDERER_H
