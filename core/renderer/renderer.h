#ifndef RENDER_ENGINE_RENDERER_H
#define RENDER_ENGINE_RENDERER_H

#include <memory>
#include <string>

class GameObject;
class Shader;
class Material;
class GraphicsBuffer;
struct Vector4;
struct Matrix4x4;
struct Bounds;
struct DrawableGeometry;

class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual Bounds                            GetAABB() const     = 0;
    virtual std::shared_ptr<DrawableGeometry> GetGeometry() const = 0;

    Matrix4x4                 GetModelMatrix() const;
    std::shared_ptr<Material> GetMaterial() const;

    bool CastShadows = true;

protected:
    Renderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Material> &_material);

    std::shared_ptr<Material> m_Material;

private:
    std::weak_ptr<GameObject> m_GameObject;
};

#endif //RENDER_ENGINE_RENDERER_H
