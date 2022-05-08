#ifndef OPENGL_STUDY_BILLBOARD_RENDERER_H
#define OPENGL_STUDY_BILLBOARD_RENDERER_H

#include "bounds/bounds.h"
#include "renderer.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>

class Texture2D;
class Point;

class BillboardRenderer: public Renderer
{
public:
    BillboardRenderer(const std::shared_ptr<GameObject> &_gameObject, std::shared_ptr<Texture2D> _texture);
    virtual ~BillboardRenderer() = default;

    Bounds                            GetAABB() const override;
    std::shared_ptr<DrawableGeometry> GetGeometry() const override;
    void                              SetRenderQueue(int _renderQueue);
    void                              SetSize(float _size);

private:
    BillboardRenderer(const BillboardRenderer &) = delete;
    BillboardRenderer(BillboardRenderer &&)      = delete;

    BillboardRenderer &operator=(const BillboardRenderer &) = delete;
    BillboardRenderer &operator=(BillboardRenderer &&) = delete;

    std::shared_ptr<Point> m_Point;
    float                  m_Aspect;
    Bounds                 m_Bounds;
};

#endif //OPENGL_STUDY_BILLBOARD_RENDERER_H
