#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include "graphics/draw_call_info.h"
#include "culling/frustum.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"
#include "enums/primitive_type.h"

#include <vector>
#include <memory>
#include <mutex>
#include <deque>
#include <shared_mutex>

class Renderer;
class RingBuffer;
class GraphicsBuffer;
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

    static void FreeMatricesEntry(const std::shared_ptr<GraphicsBufferView>& matricesBufferView);

    static bool EnableFrustumCulling;
    static bool FreezeFrustumCulling;

private:
    std::vector<DrawCallInfo> m_DrawCalls;
    const Material* m_PreviousMaterial;
    size_t m_PreviousVertexAttributesHash;
    PrimitiveType m_PreviousPrimitiveType;
    Frustum m_Frustum;

    std::vector<uint32_t> m_InstancedMatricesEntries;
    std::vector<uint32_t> m_InstancedMatricesEntriesCounts;
    std::shared_ptr<GraphicsBuffer> m_InstancedMatricesEntriesBuffer;

    std::vector<Matrix4x4> m_TemporaryMatrices;
    std::shared_ptr<GraphicsBuffer> m_TemporaryMatricesBuffer;
    std::shared_ptr<GraphicsBufferView> m_TemporaryMatricesBufferView;

    static std::mutex s_PermanentMatricesUpdatesMutex;
    static std::shared_mutex s_PermanentMatricesBufferRecreateMutex;
    static std::vector<std::pair<Matrix4x4, uint32_t>> s_PermanentMatricesUpdates;
    static std::shared_ptr<GraphicsBuffer> s_PermanentMatricesBuffer;
    static std::shared_ptr<GraphicsBufferView> s_PermanentMatricesBufferView;

    static std::mutex s_FreeMatricesBufferEntriesMutex;
    static std::deque<uint32_t> s_FreeMatricesBufferEntries;
    static uint32_t s_MatricesBufferCapacity;

    void SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& settings, const Frustum& frustum);
    void SetupDrawCalls(const std::vector<Item>& items, const RenderSettings& settings, const Frustum& frustum);
    void BatchDrawCalls();
    void SetupMatrices(const DrawCallInfo& drawCallInfo) const;
    void SetupShaderPass(const Material* material, const VertexAttributes& vertexAttributes, PrimitiveType primitiveType, uint8_t stencilValue);

    static int GetMatricesEntry();
    static void CheckMatricesBufferSize();
    static void CreatePermanentMatricesBuffer();
};

#endif //RENDER_QUEUE_H
