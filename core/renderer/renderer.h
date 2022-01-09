#ifndef OPENGL_STUDY_RENDERER_H
#define OPENGL_STUDY_RENDERER_H

#include "../../math/vector3/vector3.h"
#include <functional>
#include <memory>

class GameObject;
class Shader;

using namespace std;

class Renderer
{
public:
    virtual void Render() const = 0;

    enum class Sorting
    {
        FRONT_TO_BACK,
        BACK_TO_FRONT
    };

    struct Comparer
    {
        Sorting Sorting        = Sorting::FRONT_TO_BACK;
        Vector3 CameraPosition = Vector3();

        bool operator()(Renderer *_r1, Renderer *_r2);
    };

    struct Filter
    {
        function<bool(Renderer *)> Filter;

        static Renderer::Filter Opaque();
        static Renderer::Filter Transparent();

        bool operator()(Renderer *_renderer);
    };

protected:
    explicit Renderer(const shared_ptr<GameObject> &_gameObject);

    virtual int GetRenderQueue() const = 0;

    weak_ptr<GameObject> m_GameObject;

private:
    static constexpr int TRANSPARENT_RENDER_QUEUE = 3000;
};

#endif //OPENGL_STUDY_RENDERER_H
