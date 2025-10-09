#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include "graphics/draw_call_info.h"
#include "culling/frustum.h"

#include <vector>
#include <memory>

class Renderer;
class RingBuffer;
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

    void Draw() const;

    static bool EnableFrustumCulling;
    static bool FreezeFrustumCulling;

private:
    std::vector<DrawCallInfo> m_DrawCalls;
    Frustum m_Frustum;

    static std::shared_ptr<RingBuffer> s_InstancingMatricesBuffer;
    static std::shared_ptr<RingBuffer> s_PerDrawDataBuffer;
};

#endif //RENDER_QUEUE_H
