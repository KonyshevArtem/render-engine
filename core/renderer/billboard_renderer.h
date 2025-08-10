#ifndef RENDER_ENGINE_BILLBOARD_RENDERER_H
#define RENDER_ENGINE_BILLBOARD_RENDERER_H

#include "bounds/bounds.h"
#include "renderer.h"

class Texture2D;
class Mesh;

class BillboardRenderer: public Renderer
{
public:
    static std::shared_ptr<BillboardRenderer> Create(const nlohmann::json& componentData);

    BillboardRenderer(const std::shared_ptr<Texture2D>& texture, float size, const std::string& name);
    ~BillboardRenderer() override = default;

    Bounds GetAABB() const override;
    std::shared_ptr<DrawableGeometry> GetGeometry() override;
    void SetRenderQueue(int _renderQueue);
    void SetSize(float _size);
    void SetTexture(const std::shared_ptr<Texture2D>& texture);

    BillboardRenderer(const BillboardRenderer &) = delete;
    BillboardRenderer(BillboardRenderer &&)      = delete;

    BillboardRenderer &operator=(const BillboardRenderer &) = delete;
    BillboardRenderer &operator=(BillboardRenderer &&) = delete;

private:
    BillboardRenderer() = default;

    float m_Aspect;
    Bounds m_Bounds;
};

#endif //RENDER_ENGINE_BILLBOARD_RENDERER_H
