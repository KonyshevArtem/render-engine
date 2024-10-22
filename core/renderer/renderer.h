#ifndef RENDER_ENGINE_RENDERER_H
#define RENDER_ENGINE_RENDERER_H

#include <memory>

class GraphicsBackendBufferInfo;
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

    void SetVector(const std::string &name, const Vector4 &value);
    void SetFloat(const std::string &name, float value);
    void SetMatrix(const std::string &name, const Matrix4x4 &value);
    void SetInt(const std::string &name, int value);

    uint32_t GetInstanceDataIndex() const;
    uint32_t GetInstanceDataOffset() const;

    static const std::shared_ptr<GraphicsBuffer> &GetInstanceDataBuffer();

    bool CastShadows = true;

protected:
    Renderer(const std::shared_ptr<GameObject> &_gameObject, const std::shared_ptr<Material> &_material);

    std::shared_ptr<Material> m_Material;

private:
    void SetDataToBuffers(const std::string &name, const void *data, uint64_t size);

    std::weak_ptr<GameObject> m_GameObject;
    std::shared_ptr<GraphicsBackendBufferInfo> m_InstanceDataBufferInfo;
    int64_t m_InstanceDataBufferOffset;
};

#endif //RENDER_ENGINE_RENDERER_H
