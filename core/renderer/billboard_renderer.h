#ifndef RENDER_ENGINE_BILLBOARD_RENDERER_H
#define RENDER_ENGINE_BILLBOARD_RENDERER_H

#include "bounds/bounds.h"
#include "renderer.h"

class Texture2D;
class Mesh;

class BillboardRenderer: public Renderer
{
public:
    BillboardRenderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Texture2D> &_texture);
    ~BillboardRenderer() override = default;

    Bounds                            GetAABB() const override;
    std::shared_ptr<DrawableGeometry> GetGeometry() const override;
    void                              SetRenderQueue(int _renderQueue);
    void                              SetSize(float _size);

    BillboardRenderer(const BillboardRenderer &) = delete;
    BillboardRenderer(BillboardRenderer &&)      = delete;

    BillboardRenderer &operator=(const BillboardRenderer &) = delete;
    BillboardRenderer &operator=(BillboardRenderer &&) = delete;

private:
    std::shared_ptr<Mesh> m_Mesh;
    float                  m_Aspect;
    Bounds                 m_Bounds;
};

#endif //RENDER_ENGINE_BILLBOARD_RENDERER_H
