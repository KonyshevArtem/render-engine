#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include "graphics/draw_call_info.h"
#include "culling/frustum.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"
#include "enums/primitive_type.h"

#include <vector>
#include <memory>

class Renderer;
class RingBuffer;
class GraphicsBufferView;
struct RenderSettings;

class RenderQueue
{
public:
    struct Item
    {
        std::shared_ptr<DrawableGeometry> Geometry;
        std::shared_ptr<Material> Material;
        Matrix4x4 Matrix;
        Bounds AABB;
    };

    RenderQueue();
    ~RenderQueue() = default;

    void Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& renderSettings);
    void Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<Item>& items, const RenderSettings& renderSettings);
    void Clear();

    bool IsEmpty() const;
    const std::vector<DrawCallInfo>& GetDrawCalls() const;

    void Draw();

    static bool EnableFrustumCulling;
    static bool FreezeFrustumCulling;

private:
    std::vector<DrawCallInfo> m_DrawCalls;
    const Material* m_PreviousMaterial;
    size_t m_PreviousVertexAttributesHash;
    PrimitiveType m_PreviousPrimitiveType;
    Frustum m_Frustum;

    static std::shared_ptr<RingBuffer> s_MatricesOffsetBuffer;
    static std::shared_ptr<RingBuffer> s_MatricesBuffer;
    static std::shared_ptr<GraphicsBufferView> s_MatricesBufferView;

    static void SetupMatrices(const std::vector<Matrix4x4>& matrices);
    void SetupShaderPass(const Material* material, const VertexAttributes& vertexAttributes, PrimitiveType primitiveType, uint8_t stencilValue);
};

#endif //RENDER_QUEUE_H
