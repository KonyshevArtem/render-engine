#ifndef RENDER_ENGINE_RENDERER_H
#define RENDER_ENGINE_RENDERER_H

#include "component/component.h"

#include <memory>
#include <string>
#include <shared_mutex>

class GameObject;
class Shader;
class Material;
class GraphicsBuffer;
struct Vector4;
struct Matrix4x4;
struct Bounds;
struct DrawableGeometry;

class Renderer : public Component
{
public:
    virtual ~Renderer() = default;

    virtual Bounds GetAABB() const = 0;
    virtual std::shared_ptr<DrawableGeometry> GetGeometry() = 0;

    Matrix4x4 GetModelMatrix() const;
    std::shared_ptr<Material> GetMaterial();

    void SetMaterial(std::shared_ptr<Material> material);

    bool CastShadows = true;
    uint8_t StencilValue = 0;

protected:
    Renderer() = default;
    Renderer(const std::shared_ptr<Material> &_material);

    std::shared_ptr<Material> m_Material;
    std::shared_mutex m_MaterialMutex;
};

#endif //RENDER_ENGINE_RENDERER_H
