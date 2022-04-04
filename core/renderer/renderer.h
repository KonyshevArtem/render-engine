#ifndef OPENGL_STUDY_RENDERER_H
#define OPENGL_STUDY_RENDERER_H

#include "graphics/render_settings.h"
#include "vector3/vector3.h"
#include <functional>
#include <memory>

class GameObject;
class Shader;
struct Matrix4x4;
struct Bounds;

class Renderer
{
public:
    virtual void   Render(const RenderSettings &_settings) const  = 0;
    virtual Bounds GetAABB() const = 0;

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
        std::function<bool(Renderer *)> Delegate;

        static Renderer::Filter Opaque();
        static Renderer::Filter Transparent();
        static Renderer::Filter All();

        bool operator()(Renderer *_renderer);
    };

protected:
    explicit Renderer(const std::shared_ptr<GameObject> &_gameObject);

    Matrix4x4   GetModelMatrix() const;
    virtual int GetRenderQueue() const = 0;

private:
    static constexpr int TRANSPARENT_RENDER_QUEUE = 3000;

    std::weak_ptr<GameObject> m_GameObject;
};

#endif //OPENGL_STUDY_RENDERER_H
