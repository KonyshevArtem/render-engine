#include "render_queue.h"
#include "renderer/renderer.h"
#include "material/material.h"
#include "shader/shader.h"
#include "hash.h"
#include "graphics/render_settings/render_settings.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/ring_buffer.h"
#include "texture/texture.h"
#include "drawable_geometry/drawable_geometry.h"
#include "enums/indices_data_type.h"

bool RenderQueue::EnableFrustumCulling = true;
bool RenderQueue::FreezeFrustumCulling = false;

namespace RenderQueueLocal
{
    uint32_t k_MaxInstancingCount = 256;

    std::size_t GetDrawCallInstancingHash(const DrawCallInfo &drawCallInfo)
    {
        const std::size_t materialHash = std::hash<const Material *> {}(drawCallInfo.Material);
        const std::size_t geometryHash = std::hash<const DrawableGeometry *> {}(drawCallInfo.Geometry);
        return Hash::Combine(materialHash, geometryHash);
    }

    void SetupDrawCall(const std::shared_ptr<DrawableGeometry>& geometry, const std::shared_ptr<Material>& material, const Matrix4x4& matrix, const Bounds& aabb, bool castShadows, const RenderSettings& settings, const Frustum& frustum, std::vector<DrawCallInfo>& outDrawCalls)
    {
        const Material* mat = settings.OverrideMaterial ? settings.OverrideMaterial.get() : material.get();

        if (!geometry || !mat)
            return;

        if (RenderQueue::EnableFrustumCulling && !frustum.IsVisible(aabb, settings.FrustumCullingPlanesBits))
            return;

        DrawCallInfo info{
                geometry.get(),
                mat,
                {matrix},
                aabb,
                castShadows,
                false
        };
        outDrawCalls.push_back(info);
    }

    void SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& settings, const Frustum& frustum, std::vector<DrawCallInfo>& outDrawCalls)
    {
        outDrawCalls.reserve(renderers.size());

        for (const std::shared_ptr<Renderer>& renderer : renderers)
        {
            if (renderer)
                SetupDrawCall(renderer->GetGeometry(), renderer->GetMaterial(), renderer->GetModelMatrix(), renderer->GetAABB(), renderer->CastShadows, settings, frustum, outDrawCalls);
        }
    }

    void SetupDrawCalls(const std::vector<RenderQueue::Item>& items, const RenderSettings& settings, const Frustum& frustum, std::vector<DrawCallInfo>& outDrawCalls)
    {
        outDrawCalls.reserve(items.size());

        for (const RenderQueue::Item& item : items)
            SetupDrawCall(item.Geometry, item.Material, item.Matrix, item.AABB, false, settings, frustum, outDrawCalls);
    }

    void FilterDrawCalls(const DrawCallFilter& filter, std::vector<DrawCallInfo>& outDrawCalls)
    {
        for (size_t i = 0; i < outDrawCalls.size(); ++i)
        {
            const DrawCallInfo& drawCall = outDrawCalls[i];
            if (!filter(drawCall))
            {
                outDrawCalls[i] = outDrawCalls[outDrawCalls.size() - 1];
                outDrawCalls.pop_back();
                --i;
            }
        }
    }

    void BatchDrawCalls(std::vector<DrawCallInfo>& outDrawCalls)
    {
        std::unordered_map<std::size_t, DrawCallInfo*> instancingMap;

        for (size_t i = 0; i < outDrawCalls.size(); ++i)
        {
            DrawCallInfo& drawCall = outDrawCalls[i];
            if (!drawCall.Material->GetShader()->SupportInstancing())
            {
                continue;
            }

            const size_t hash = RenderQueueLocal::GetDrawCallInstancingHash(drawCall);
            const auto it = instancingMap.find(hash);

            if (it == instancingMap.end())
            {
                drawCall.Instanced = true;
                instancingMap[hash] = &drawCall;
                continue;
            }

            DrawCallInfo* instancedDrawCall = it->second;
            instancedDrawCall->ModelMatrices.push_back(drawCall.ModelMatrices[0]);
            instancedDrawCall->AABB = instancedDrawCall->AABB.Combine(drawCall.AABB);

            outDrawCalls[i] = outDrawCalls[outDrawCalls.size() - 1];
            outDrawCalls.pop_back();
            --i;

            if (instancedDrawCall->ModelMatrices.size() >= RenderQueueLocal::k_MaxInstancingCount)
                instancingMap.erase(it);
        }
    }

    void SortDrawCalls(DrawCallSortMode sortMode, const Matrix4x4& viewProjectionMatrix, std::vector<DrawCallInfo>& outDrawCalls)
    {
        const Matrix4x4 invViewProjection = viewProjectionMatrix.Invert();
        const Vector4 cameraDirection = invViewProjection * Vector4(0, 0, 1, 0);

        if (sortMode != DrawCallSortMode::NO_SORTING)
            std::sort(outDrawCalls.begin(), outDrawCalls.end(), DrawCallComparer {sortMode, cameraDirection});
    }

    void SetupMatrices(const Matrix4x4 &modelMatrix, std::shared_ptr<RingBuffer> buffer)
    {
        Matrix4x4 matrices[2];
        matrices[0] = modelMatrix;
        matrices[1] = modelMatrix.Invert().Transpose();
        buffer->SetData(&matrices[0], 0, sizeof(matrices));

        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, buffer->GetCurrentElementOffset(), sizeof(matrices));
    }

    void SetupMatrices(const std::vector<Matrix4x4> &matrices, std::shared_ptr<RingBuffer> buffer)
    {
        static std::vector<Matrix4x4> matricesBuffer;

        int count = matrices.size();

        matricesBuffer.resize(count * 2);
        for (int i = 0; i < count; ++i)
        {
            matricesBuffer[i * 2 + 0] = matrices[i];
            matricesBuffer[i * 2 + 1] = matrices[i].Invert().Transpose();
        }

        int matricesSize = sizeof(Matrix4x4) * matricesBuffer.size();
        buffer->SetData(matricesBuffer.data(), 0, matricesSize);

        GraphicsBackend::Current()->BindStructuredBuffer(buffer->GetBackendBuffer(), 0, buffer->GetCurrentElementOffset(), matricesSize, count);
    }

    void SetupShaderPass(const Material* material, const VertexAttributes &vertexAttributes, PrimitiveType primitiveType)
    {
        const std::shared_ptr<GraphicsBuffer>& perMaterialDataBuffer = material->GetPerMaterialDataBuffer();
        if (perMaterialDataBuffer)
            GraphicsBackend::Current()->BindConstantBuffer(perMaterialDataBuffer->GetBackendBuffer(), 4, 0, perMaterialDataBuffer->GetSize());

        for (const auto& pair: material->GetTextures())
        {
            uint32_t binding = pair.first;
            const std::shared_ptr<Texture> &texture = pair.second;

            GraphicsBackend::Current()->BindTextureSampler(texture->GetBackendTexture(), texture->GetBackendSampler(), binding);
        }

        GraphicsBackend::Current()->UseProgram(material->GetShader()->GetProgram(vertexAttributes, primitiveType));
    }
}

std::shared_ptr<RingBuffer> RenderQueue::s_InstancingMatricesBuffer;
std::shared_ptr<RingBuffer> RenderQueue::s_PerDrawDataBuffer;

RenderQueue::RenderQueue()
{
    if (!s_InstancingMatricesBuffer)
    {
        int matricesBufferSize = sizeof(Matrix4x4) * RenderQueueLocal::k_MaxInstancingCount * 2;
        s_InstancingMatricesBuffer = std::make_shared<RingBuffer>(matricesBufferSize, 64, "PerInstanceMatrices");
    }

    if (!s_PerDrawDataBuffer)
        s_PerDrawDataBuffer = std::make_shared<RingBuffer>(sizeof(Matrix4x4) * 2, 1024, "PerDrawData");
}

void RenderQueue::Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& renderSettings)
{
    m_DrawCalls.clear();

    if (!FreezeFrustumCulling)
        m_Frustum = Frustum(viewProjectionMatrix);

    RenderQueueLocal::SetupDrawCalls(renderers, renderSettings, m_Frustum, m_DrawCalls);
    RenderQueueLocal::FilterDrawCalls(renderSettings.Filter, m_DrawCalls);
    RenderQueueLocal::BatchDrawCalls(m_DrawCalls);
    RenderQueueLocal::SortDrawCalls(renderSettings.Sorting, viewProjectionMatrix, m_DrawCalls);
}

void RenderQueue::Prepare(const Matrix4x4& viewProjectionMatrix, const std::vector<Item>& items, const RenderSettings& renderSettings)
{
    m_DrawCalls.clear();

    if (!FreezeFrustumCulling)
        m_Frustum = Frustum(viewProjectionMatrix);

    RenderQueueLocal::SetupDrawCalls(items, renderSettings, m_Frustum, m_DrawCalls);
    RenderQueueLocal::FilterDrawCalls(renderSettings.Filter, m_DrawCalls);
    RenderQueueLocal::BatchDrawCalls(m_DrawCalls);
    RenderQueueLocal::SortDrawCalls(renderSettings.Sorting, viewProjectionMatrix, m_DrawCalls);
}

void RenderQueue::Clear()
{
    m_DrawCalls.clear();
}

bool RenderQueue::IsEmpty() const
{
    return m_DrawCalls.empty();
}

const std::vector<DrawCallInfo>& RenderQueue::GetDrawCalls() const
{
    return m_DrawCalls;
}

void RenderQueue::Draw() const
{
    for (const DrawCallInfo& drawCall : m_DrawCalls)
    {
        const GraphicsBackendGeometry& geom = drawCall.Geometry->GetGraphicsBackendGeometry();
        const PrimitiveType primitiveType = drawCall.Geometry->GetPrimitiveType();
        const int elementsCount = drawCall.Geometry->GetElementsCount();
        const bool hasIndices = drawCall.Geometry->HasIndexes();

        if (drawCall.Instanced)
            RenderQueueLocal::SetupMatrices(drawCall.ModelMatrices, s_InstancingMatricesBuffer);
        else
            RenderQueueLocal::SetupMatrices(drawCall.ModelMatrices[0], s_PerDrawDataBuffer);

        RenderQueueLocal::SetupShaderPass(drawCall.Material, drawCall.Geometry->GetVertexAttributes(), primitiveType);

        if (drawCall.Instanced)
        {
            const int instanceCount = drawCall.ModelMatrices.size();
            if (hasIndices)
                GraphicsBackend::Current()->DrawElementsInstanced(geom, primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT, instanceCount);
            else
                GraphicsBackend::Current()->DrawArraysInstanced(geom, primitiveType, 0, elementsCount, instanceCount);
        }
        else
        {
            if (hasIndices)
                GraphicsBackend::Current()->DrawElements(geom, primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT);
            else
                GraphicsBackend::Current()->DrawArrays(geom, primitiveType, 0, elementsCount);
        }
    }
}